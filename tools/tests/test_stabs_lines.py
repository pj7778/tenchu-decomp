from __future__ import annotations

import unittest

from tools import stabs_lines


class StabsLinesTests(unittest.TestCase):
    def test_drops_crash_inducing_type_stabs_keeps_line_records(self) -> None:
        source = [
            '.file\t1 "src/main.exe/PadProc.c"\n',
            '.stabs "src/main.exe/PadProc.c",100,0,0,$Ltext0\n',   # N_SO keep
            '.stabs "int:t1=r1;-2147483648;2147483647;",128,0,0,0\n',  # type drop
            '.stabs "unsigned int:t4=r4;0;-1;",128,0,0,0\n',           # crash drop
            '.stabs "PadProc:F1",36,0,0,PadProc\n',                # N_FUN keep
            "\t.text\n",
            "PadProc:\n",
            ".stabn 68,0,103,$LM1\n",                              # N_SLINE keep
            "\taddiu $sp,$sp,-24\n",
            '.stabs "ct:1",128,0,0,-20\n',                         # local var drop
            ".stabn 68,0,106,$LM2\n",                              # N_SLINE keep
            "\tjr $ra\n",
        ]
        out = stabs_lines.filter_lines(source)
        text = "".join(out)
        # Type/var stabs that crash modern gdb are gone.
        self.assertNotIn("unsigned int:t4", text)
        self.assertNotIn(":t1=r1", text)
        self.assertNotIn('"ct:1"', text)
        # Line/file/function records and all real code survive.
        self.assertIn('.stabs "src/main.exe/PadProc.c",100', text)
        self.assertIn('.stabs "PadProc:F1",36', text)
        self.assertEqual(text.count(".stabn 68,"), 2)
        self.assertIn("addiu $sp,$sp,-24", text)
        self.assertIn("jr $ra", text)
        self.assertIn(".file\t1", text)

    def test_non_stab_content_is_untouched(self) -> None:
        source = ["\tlw $v0,0($a0)\n", "\tnop\n", ".word 0x12345678\n"]
        self.assertEqual(stabs_lines.filter_lines(source), source)

    def test_n_sline_is_the_only_stabn_kept(self) -> None:
        # 68 is N_SLINE; other .stabn codes (e.g. N_LBRAC 192) are dropped.
        source = [".stabn 68,0,10,$LM1\n", ".stabn 192,0,0,$LBB2\n"]
        out = stabs_lines.filter_lines(source)
        self.assertEqual(out, [".stabn 68,0,10,$LM1\n"])


if __name__ == "__main__":
    unittest.main()
