#include <stdlib.h>
#include <stdio.h>

#include "parser/symbols.h"
#include "parser/parser.h"

int main(int argc, char **argv) {

    // Open assembly file.
    assert(argc > 1);
    FILE *file = fopen(argv[1], "r");
    // If file does not exist, it has not been opened successfully.
    if (!file) {
       return FILE_OPEN_ERROR;
    }

    // Completes first pass and rewinds file.
    SymbolMap* symbolMap = new_symbol_map(0);
    AssemblyInfo assemblyInfo = collect_symbols(symbolMap, file);


    translate(symbolMap, file);
    





    return EXIT_SUCCESS;
}
