from __future__ import annotations

import unittest

from tools import pcsx_gameplay


class PcsxGameplayTests(unittest.TestCase):
    def test_probe_symbols_cover_the_documented_gameplay_anchors(self) -> None:
        self.assertEqual(
            pcsx_gameplay.PROBE_SYMBOLS,
            {
                "MAIN": "main",
                "LOOP": "PadProc",
                "CREATESTAGE": "CreateStage",
                "ACTIVATE": "ActivateHumans",
                "ACTSTATE": "ActSTATE",
                "ACTNORMAL": "ActNORMAL",
                "CBCHECK": "cbCheckCD",
            },
        )

    def test_repack_and_disc_are_mutually_exclusive(self) -> None:
        with self.assertRaises(SystemExit):
            pcsx_gameplay.parse_args(["--repack", "--disc", "/tmp/x.cue"])

    def test_thresholds_must_be_positive(self) -> None:
        for flags in (["--act-min", "0"], ["--cb-min", "-1"], ["--max-vsyncs", "0"]):
            with self.assertRaises(SystemExit):
                pcsx_gameplay.parse_args(flags)

    def test_defaults_target_the_growth_artifact(self) -> None:
        args = pcsx_gameplay.parse_args([])
        self.assertEqual(args.exe.name, "main_growth.exe")
        self.assertEqual(args.elf.name, "main_growth.exe.elf")
        self.assertEqual(args.act_min, 600)
        self.assertEqual(args.cb_min, 300)


if __name__ == "__main__":
    unittest.main()
