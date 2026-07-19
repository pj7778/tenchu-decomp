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


if __name__ == "__main__":
    unittest.main()
