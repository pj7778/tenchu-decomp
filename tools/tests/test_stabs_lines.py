from __future__ import annotations

import unittest

from tools import stabs_lines


class StabsFixupTests(unittest.TestCase):
    def test_patches_the_three_crash_forms_by_width(self) -> None:
        source = [
            '.stabs "unsigned int:t4=r4;0;-1;",128,0,0,0\n',
            '.stabs "unsigned char:t11=r11;0;-1;",128,0,0,0\n',
            '.stabs "short unsigned int:t9=r9;0;-1;",128,0,0,0\n',
            '.stabs "long long int:t6=r6;0;-1;",128,0,0,0\n',
            '.stabs "float:t12=r1;4;0;",128,0,0,0\n',
            '.stabs "buf:t50=ar0;0;39;2",128,0,0,0\n',
        ]
        out = "".join(stabs_lines.fixup_stabs(source))
        self.assertIn("unsigned int:t4=r4;0;037777777777;", out)
        self.assertIn("unsigned char:t11=r11;0;255;", out)
        self.assertIn("short unsigned int:t9=r9;0;65535;", out)
        self.assertIn("long long int:t6=r6;0;01777777777777777777777;", out)
        self.assertIn("float:t12=r1;-2147483648;2147483647;", out)
        self.assertIn("buf:t50=ar1;0;39;2", out)  # index type 0 -> 1
        # No degenerate form survives.
        self.assertNotIn(";0;-1;", out)
        self.assertNotIn("=ar0;", out)

    def test_keeps_locals_functions_and_lines(self) -> None:
        # Everything that does not crash is preserved verbatim, so gdb still
        # gets locals (ct), functions, and line records.
        source = [
            '.stabs "ProcItemKusuri:F19",36,0,0,ProcItemKusuri\n',
            '.stabs "item:p20",160,0,0,0\n',
            '.stabs "ct:1",128,0,0,-20\n',
            ".stabn 68,0,103,$LM1\n",
            "\taddiu $sp,$sp,-24\n",
        ]
        self.assertEqual(stabs_lines.fixup_stabs(source), source)

    def test_non_stab_lines_untouched(self) -> None:
        source = ["\tlw $v0,0($a0)\n", ".word 0x12345678\n"]
        self.assertEqual(stabs_lines.fixup_stabs(source), source)

    def test_signed_ranges_are_left_alone(self) -> None:
        source = ['.stabs "int:t1=r1;-2147483648;2147483647;",128,0,0,0\n',
                  '.stabs "char:t2=r2;0;255;",128,0,0,0\n']
        self.assertEqual(stabs_lines.fixup_stabs(source), source)


if __name__ == "__main__":
    unittest.main()
