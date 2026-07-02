// Export all global symbols + all data types, for import into the decomp repo.
// Writes, in the chosen output directory:
//   symbols.tsv   one line per global function/data symbol:
//                 <hexaddr>\t<FUNCTION|DATA>\t<sizeBytes>\t<name>
//   types.h       every data type as C (via Ghidra's DataTypeWriter)
//
// Run on the program holding main.exe (loaded at 0x80011000):
//   analyzeHeadless <proj_dir> <proj>/<folder> -process MAIN.EXE -readOnly \
//     -noanalysis -scriptPath tools/ghidra -postScript ExportSymbolsTypes.java <outDir>
//
//@author tenchu-decomp
//@category Data Types

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.data.DataTypeWriter;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;
import ghidra.program.model.symbol.SymbolType;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.Writer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.Iterator;

public class ExportSymbolsTypes extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File outDir = (args.length > 0) ? new File(args[0])
                                        : askDirectory("Export output directory", "Select");
        outDir.mkdirs();

        // --- symbols ---
        SymbolTable st = currentProgram.getSymbolTable();
        FunctionManager fm = currentProgram.getFunctionManager();
        StringBuilder sb = new StringBuilder();
        int nfun = 0, ndata = 0;
        SymbolIterator it = st.getSymbolIterator(true);
        while (it.hasNext() && !monitor.isCancelled()) {
            Symbol sym = it.next();
            if (!sym.isGlobal() || sym.isExternal()) {
                continue;
            }
            SymbolType t = sym.getSymbolType();
            String kind;
            long size = 0;
            if (t == SymbolType.FUNCTION) {
                kind = "FUNCTION";
                Function f = fm.getFunctionAt(sym.getAddress());
                if (f != null) {
                    size = f.getBody().getNumAddresses();
                }
                nfun++;
            } else if (t == SymbolType.LABEL) {
                kind = "DATA";
                ndata++;
            } else {
                continue;
            }
            sb.append(String.format("%08x\t%s\t%d\t%s%n",
                    sym.getAddress().getOffset(), kind, size, sym.getName()));
        }
        Files.write(new File(outDir, "symbols.tsv").toPath(),
                    sb.toString().getBytes(StandardCharsets.UTF_8));

        // --- data types ---
        int ntypes = 0;
        try (Writer w = new BufferedWriter(new FileWriter(new File(outDir, "types.h")))) {
            DataTypeManager dtm = currentProgram.getDataTypeManager();
            DataTypeWriter dtw = new DataTypeWriter(dtm, w);
            dtw.write(dtm, monitor);
            Iterator<DataType> ti = dtm.getAllDataTypes();
            while (ti.hasNext()) { ti.next(); ntypes++; }
        }

        println(String.format("ExportSymbolsTypes: %d functions, %d data symbols, "
                + "%d data types -> %s", nfun, ndata, ntypes, outDir));
    }
}
