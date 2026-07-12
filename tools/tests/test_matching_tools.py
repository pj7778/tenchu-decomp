#!/usr/bin/env python3
import os
import sys
import tempfile
import unittest
from unittest import mock
import contextlib
import io

TOOLS = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if TOOLS not in sys.path:
    sys.path.insert(0, TOOLS)

import autorules
import permute
import regalloc
import rtlguide


class AutoRulesAdvancedTests(unittest.TestCase):
    def candidates(self, rule, source):
        return list(rule(source, "F", (0, len(source))))

    def test_abs_copy_barrier(self):
        source = """int F(int value) {
    int absolute;
    absolute = value;
    if (value < 0) {
        absolute = -absolute;
    }
    return absolute;
}
"""
        out = self.candidates(autorules.rule_abs_copy_barrier, source)
        self.assertEqual(len(out), 1)
        self.assertIn('__asm__("" : "+r"(absolute));', out[0][1])

    def test_copy_barrier_tries_both_sides(self):
        source = """int F(int a) {
    int b;
    b = a;
    a += 1;
    return a + b;
}
"""
        out = self.candidates(autorules.rule_copy_barrier, source)
        self.assertEqual(len(out), 2)
        self.assertTrue(any('"+r"(a)' in text for _, text in out))
        self.assertTrue(any('"+r"(b)' in text for _, text in out))

    def test_case_fence_unwraps_else_clause(self):
        source = """int F(int a) {
    if (a) { a++; } else { a--; }
    return a;
}
"""
        out = self.candidates(autorules.rule_case_fence, source)
        self.assertEqual(len(out), 2)
        for _, text in out:
            self.assertIn("switch (!!(a))", text)
            self.assertNotIn("{ else", text)

    def test_case_fence_rejects_break_semantics(self):
        source = """int F(int a) {
    while (a) {
        if (a == 1) { break; } else { a--; }
    }
    return a;
}
"""
        self.assertEqual(self.candidates(autorules.rule_case_fence, source), [])

    def test_hard_clobber_is_numeric(self):
        source = """int F(int a) {
    int b;
    b = a;
    return b;
}
"""
        out = self.candidates(autorules.hard_clobber_rule(["v0"]), source)
        self.assertEqual(len(out), 1)
        self.assertIn('__asm__("" ::: "$2");', out[0][1])

    def test_beam_retains_neutral_enabling_edit(self):
        def first(text, _name, _span):
            if text == "A":
                yield "first", "B"

        def second(text, _name, _span):
            if text == "B":
                yield "second", "C"

        scores = {"B": (False, 10, None, None), "C": (True, 0, None, None)}
        with tempfile.NamedTemporaryFile("w+", delete=False) as f:
            path = f.name
            f.write("A")
        try:
            def fake_score(_name, _partial):
                with open(path) as f:
                    return scores[f.read()]

            rules = [("first", "", first), ("second", "", second)]
            with mock.patch.object(autorules, "score", side_effect=fake_score):
                with contextlib.redirect_stdout(io.StringIO()):
                    text, score, matched, applied = autorules.beam_search(
                        path, "A", "F", False, rules, 10, width=2, depth=2,
                        allow_regress=0, budget=4,
                    )
            self.assertEqual((text, score, matched), ("C", 0, True))
            self.assertEqual([x[0] for x in applied], ["first", "second"])
        finally:
            os.unlink(path)


class RtlGuideTests(unittest.TestCase):
    @staticmethod
    def hunk(target, ours):
        return {
            "target": [(0x1000 + i * 4, x) for i, x in enumerate(target)],
            "ours": [(0x1000 + i * 4, x) for i, x in enumerate(ours)],
        }

    def test_register_rotation_is_regalloc(self):
        h = self.hunk(["addu v0,a0,a1", "sw v0,0(s0)"],
                      ["addu v1,a1,a0", "sw v1,0(s1)"])
        self.assertEqual(rtlguide.classify_hunk(h), "regalloc")

    def test_missing_move_is_cse(self):
        h = self.hunk(["move v0,a0"], ["nop"])
        self.assertEqual(rtlguide.classify_hunk(h), "cse/coalescing")

    def test_reorder_is_scheduler(self):
        h = self.hunk(["lw v0,0(a0)", "addiu v1,v1,1"],
                      ["addiu v1,v1,1", "lw v0,0(a0)"])
        self.assertEqual(rtlguide.classify_hunk(h), "schedule/delay")

    def test_branch_target_drift_has_same_shape(self):
        self.assertEqual(rtlguide.shape("bnez v0,0x80012340"),
                         rtlguide.shape("bnez v1,0x80012400"))

    def test_json_keeps_hunk_instructions_not_full_streams(self):
        report = {
            "target": [(1, "nop")], "ours": [(1, "move v0,v1")],
            "hunks": [{
                "target": [(1, "nop")], "ours": [(1, "move v0,v1")],
                "target_indices": [0], "ours_indices": [0],
            }],
        }
        out = rtlguide._jsonable(report)
        self.assertNotIn("target", out)
        self.assertEqual(out["hunks"][0]["target"], [[1, "nop"]])


class RegallocParserTests(unittest.TestCase):
    def test_preferences_are_exposed(self):
        body = """;; 80 conflicts: 80 2 3
;; 80 preferences: 2
;; Register dispositions:
80 in 3
;; Hard regs used: 2 3
"""
        result = regalloc.analyse("F", body)
        self.assertEqual(result["disp"][80], 3)
        self.assertEqual(result["preferences"][80], [2])
        self.assertEqual(result["conflicts"][80], [80, 2, 3])


class BuildConfigurationTests(unittest.TestCase):
    def test_expand_div_table_matches_build(self):
        path = os.path.join(os.path.dirname(TOOLS), "shake", "src", "Build.hs")
        with open(path) as f:
            build = f.read()
        block = build.split("maspsxGpExterns src =", 1)[1].split("extra _ = []", 1)[0]
        names = set(__import__("re").findall(
            r'extra "([A-Za-z0-9_]+)" = \["--expand-div"\]', block
        ))
        self.assertEqual(names, set(permute.MASPSX_EXTRA))

    def test_gp_extern_table_matches_build(self):
        path = os.path.join(os.path.dirname(TOOLS), "shake", "src", "Build.hs")
        with open(path) as f:
            build = f.read()
        block = build.split("maspsxGpExterns src =", 1)[1].split("syms _ = []", 1)[0]
        found = {}
        pattern = __import__("re").compile(
            r'^\s*syms "([A-Za-z0-9_]+)" = \[(.*)\]$', __import__("re").M
        )
        for name, values in pattern.findall(block):
            found[name] = __import__("re").findall(r'"([^"]+)"', values)
        self.assertEqual(found, permute.GP_EXTERNS)


if __name__ == "__main__":
    unittest.main()
