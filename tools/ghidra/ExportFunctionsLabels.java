// Export a program's functions and named labels as two TSVs.
//
// Used to produce reference/demo-psxexe.functions.tsv and .labels.tsv from the
// demo's PSX.EXE inside the Ghidra project -- the name tables that
// tools/xbuildnames.py, tools/callmatch.py and tools/datamatch.py match against.
// (Those names are boricj's propagation of PSX.SYM onto PSX.EXE; every tool
// re-filters them through disks/demo/PSX.SYM for provenance.)
//
// headless:
//   analyzeHeadless <proj_dir> <proj_name>/<folder> -process 'PSX.EXE' \
//     -noanalysis -readOnly -scriptPath tools/ghidra \
//     -postScript ExportFunctionsLabels.java <funcsTsv> <labelsTsv>
//
// The demo PSX.EXE lives at:
//   tenchu-decompile / "Rittai Ninja Katsugeki - Tenchu (Japan) (Demo)/TENCHU.VOL/CDIMAGE"
//
// functions.tsv: <hexaddr>\t<sizeBytes>\t<name>
// labels.tsv:    <hexaddr>\t<name>          (non-default LABEL symbols only)
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolType;
import ghidra.program.model.symbol.SourceType;
import java.io.FileWriter;
import java.io.PrintWriter;

public class ExportFunctionsLabels extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        if (a.length < 2) {
            println("usage: ExportFunctionsLabels <funcsTsv> <labelsTsv>");
            return;
        }
        int nf = 0, nl = 0;
        try (PrintWriter w = new PrintWriter(new FileWriter(a[0]))) {
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                w.printf("%08x\t%d\t%s%n", f.getEntryPoint().getOffset(),
                         f.getBody().getNumAddresses(), f.getName());
                nf++;
            }
        }
        try (PrintWriter w = new PrintWriter(new FileWriter(a[1]))) {
            for (Symbol s : currentProgram.getSymbolTable().getAllSymbols(false)) {
                if (s.getSource() == SourceType.DEFAULT) continue;
                if (s.getSymbolType() != SymbolType.LABEL) continue;
                w.printf("%08x\t%s%n", s.getAddress().getOffset(), s.getName());
                nl++;
            }
        }
        println("wrote " + nf + " functions and " + nl + " labels");
    }
}
