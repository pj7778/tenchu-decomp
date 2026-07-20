from __future__ import annotations

import unittest

from tools import xexe


def hit(name: str, target: int, size: int, src: int = 0x80011000):
    return (name, src, target, size)


class MergeHitsTests(unittest.TestCase):
    def test_main_names_take_precedence_over_overlapping_demo_hits(self) -> None:
        main_hits = [hit("PadProc", 0x80020000, 0x100)]
        demo_hits = [hit("DemoName", 0x80020040, 0x40)]  # inside PadProc
        new, agreements, disagreements, covered, contested = xexe.merge_hits(
            main_hits, demo_hits
        )
        self.assertEqual(new, [])
        self.assertEqual(agreements, 0)
        self.assertEqual(disagreements, [])
        self.assertEqual(len(covered), 1)
        self.assertEqual(contested, 0)

    def test_span_identical_same_name_counts_as_agreement(self) -> None:
        main_hits = [hit("SpuInit", 0x80030000, 0x80)]
        demo_hits = [hit("SpuInit", 0x80030000, 0x80)]
        new, agreements, disagreements, covered, contested = xexe.merge_hits(
            main_hits, demo_hits
        )
        self.assertEqual((new, agreements, disagreements, covered, contested),
                         ([], 1, [], [], 0))

    def test_span_identical_different_name_is_reported_not_resolved(self) -> None:
        main_hits = [hit("SpuRead", 0x80030000, 0x80)]
        demo_hits = [hit("SpuWrite", 0x80030000, 0x80)]
        new, agreements, disagreements, covered, contested = xexe.merge_hits(
            main_hits, demo_hits
        )
        self.assertEqual(new, [])
        self.assertEqual(disagreements, [(0x80030000, "SpuRead", "SpuWrite")])

    def test_uncovered_demo_hit_is_a_new_name(self) -> None:
        main_hits = [hit("PadProc", 0x80020000, 0x100)]
        demo_hits = [hit("strInit", 0x80040000, 0x70)]
        new, agreements, disagreements, covered, contested = xexe.merge_hits(
            main_hits, demo_hits
        )
        self.assertEqual(new, [hit("strInit", 0x80040000, 0x70)])

    def test_contested_demo_target_is_dropped(self) -> None:
        demo_hits = [
            hit("dmyA", 0x80040000, 0x40),
            hit("dmyB", 0x80040000, 0x40),
        ]
        new, agreements, disagreements, covered, contested = xexe.merge_hits(
            [], demo_hits
        )
        self.assertEqual(new, [])
        self.assertEqual(contested, 1)

    def test_duplicate_demo_claims_with_one_name_survive(self) -> None:
        demo_hits = [
            hit("strNext", 0x80040000, 0x40, src=0x80010100),
            hit("strNext", 0x80040000, 0x40, src=0x80010200),
        ]
        new, *_ = xexe.merge_hits([], demo_hits)
        self.assertEqual(len(new), 1)
        self.assertEqual(new[0][0], "strNext")


class CallPlacementHelperTests(unittest.TestCase):
    def test_jal_target_decodes_within_the_pc_region(self) -> None:
        word = 0x0C000000 | ((0x80014548 & 0x0FFFFFFF) >> 2)
        self.assertEqual(xexe.jal_target(word, 0x80020000), 0x80014548)
        self.assertIsNone(xexe.jal_target(0x27BDFFE8, 0x80020000))  # addiu

    def test_named_callees_collects_only_named_targets_in_range(self) -> None:
        import struct

        base = 0x80011000
        words = [
            0x0C000000 | ((0x80011100 & 0x0FFFFFFF) >> 2),  # jal named
            0x00000000,
            0x0C000000 | ((0x80011200 & 0x0FFFFFFF) >> 2),  # jal unnamed
            0x03E00008,
        ]
        blob = bytes(0x800) + b"".join(struct.pack("<I", w) for w in words)
        names = {0x80011100: "StSetRing"}
        self.assertEqual(
            xexe.named_callees(blob, base, base, len(words) * 4, names),
            ("StSetRing",),
        )

    def test_candidates_are_jal_targets_split_at_anchors(self) -> None:
        import struct

        base = 0x80011000
        # One jal to base+0x20 (a candidate), one to an anchored address.
        words = [
            0x0C000000 | (((base + 0x20) & 0x0FFFFFFF) >> 2),
            0x0C000000 | (((base + 0x40) & 0x0FFFFFFF) >> 2),
        ] + [0] * 30
        blob = bytes(0x800) + b"".join(struct.pack("<I", w) for w in words)
        anchors = {base + 0x40: (0x10, "Named")}
        candidates = xexe.candidate_functions(blob, base, anchors)
        self.assertIn((base + 0x20, 0x20), candidates)  # capped at the anchor
        self.assertNotIn(base + 0x40, [c[0] for c in candidates])


if __name__ == "__main__":
    unittest.main()
