#!/usr/bin/env python3
import os
import sys
import tempfile
import unittest
from unittest import mock
import contextlib
import io
import inspect

TOOLS = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if TOOLS not in sys.path:
    sys.path.insert(0, TOOLS)

import autorules
import permute
import regalloc
import rtlguide


class AutoRulesAdvancedTests(unittest.TestCase):
    def setUp(self):
        autorules.GUIDED_LINES = set()

    def candidates(self, rule, source):
        return list(rule(source, "F", (0, len(source))))

    def test_cmp_polarity_swaps_local_operands(self):
        source = """int F(int direction, int turn) {
    if (direction < turn) return 1;
    return 0;
}
"""
        out = self.candidates(autorules.rule_cmp_polarity, source)
        self.assertEqual(len(out), 1)
        self.assertIn("turn > direction", out[0][1])

    def test_loop_fence_wraps_an_if(self):
        source = """int F(int value) {
    if (value) value++;
    return value;
}
"""
        out = self.candidates(autorules.rule_loop_fence, source)
        self.assertEqual(len(out), 1)
        self.assertIn("do {", out[0][1])
        self.assertIn("} while (0);", out[0][1])

    def test_loop_fence_rejects_if_with_switch_break(self):
        source = """int F(int a) {
    switch (a) {
    case 1:
        if (a) break;
    }
    return a;
}
"""
        self.assertEqual(self.candidates(autorules.rule_loop_fence, source), [])

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

    def test_registered_rules_never_emit_inline_asm(self):
        self.assertNotIn("__asm__", inspect.getsource(autorules))
        for key, _description, rule in autorules.RULES + autorules.AGGRESSIVE_RULES:
            with self.subTest(rule=key):
                self.assertNotIn("__asm__", inspect.getsource(rule))

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

    def test_target_only_call_sets_call_tail_hint(self):
        target = [(0x1000, "jal 0x80001000 <DeleteConflict>"),
                  (0x1004, "nop")]
        ours = [(0x1000, "j 0x80002000"), (0x1004, "nop")]
        with mock.patch.object(
                rtlguide, "_candidate_asm",
                return_value=(0x1000, 8, 8, target, ours)):
            guide = rtlguide.assembly_guide("F")
        self.assertTrue(guide["call_tail_hint"])
        self.assertEqual(guide["call_counts"], {"target": 1, "candidate": 0})
        self.assertEqual(guide["target_only_calls"], [
            {"callee": "DeleteConflict", "count": 1},
        ])

    def test_call_rtl_fingerprint_includes_result_mode_and_usage(self):
        body = """;; Function F

(call_insn 10 9 11 (parallel[
            (set (reg:HI 2 v0)
                (call (mem:SI (symbol_ref:SI (\"DeleteConflict\")))
                    (const_int 16)))
            (clobber (reg:SI 31 ra))
        ] ) -1 (nil)
    (nil)
    (expr_list (use (reg:SI 5 a1))
        (expr_list (use (reg:SI 4 a0))
            (nil))))

;; Function Other

(call_insn 20 19 21 (parallel[
            (call (mem:SI (symbol_ref:SI ("Unrelated"))) (const_int 16))
            (clobber (reg:SI 31 ra))
        ] ) -1 (nil) (nil) (nil))
"""
        with tempfile.NamedTemporaryFile("w+", delete=False) as f:
            path = f.name
            f.write(body)
        try:
            self.assertEqual(rtlguide.parse_call_rtl(path, "F"), [{
                "callee": "DeleteConflict", "result": "HI",
                "uses": ["SI:$a1", "SI:$a0"],
            }])
        finally:
            os.unlink(path)

    def test_call_rtl_evidence_tracks_jump_merging(self):
        before = [
            {"callee": "DeleteConflict", "result": "void", "uses": ["SI:$a0"]},
            {"callee": "DeleteConflict", "result": "HI", "uses": ["SI:$a0"]},
        ]
        after = [before[0]]
        with mock.patch.object(rtlguide, "parse_call_rtl",
                               side_effect=[before, after]):
            evidence = rtlguide.call_rtl_evidence(
                ["F.rtl", "F.jump2"],
                [{"callee": "DeleteConflict", "count": 1}], "F",
            )
        self.assertEqual(evidence[0]["expanded_sites"], 2)
        self.assertEqual(evidence[0]["after_jump_sites"], 1)
        self.assertEqual({x["result"] for x in evidence[0]["fingerprints"]},
                         {"void", "HI"})

    def test_guided_registry_is_pure_c(self):
        rules = {rule for values in rtlguide.CATEGORY_RULES.values() for rule in values}
        self.assertIn("cmp-polarity", rules)
        self.assertIn("loop-fence", rules)
        self.assertIn("type-width", rules)
        self.assertFalse(any("barrier" in rule or "clobber" in rule for rule in rules))

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
