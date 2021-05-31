#include <stdlib.h>
#include <stdio.h>

#include "parser/symbols.h"
#include "parser/parser.h"
#include "translate.h"
#include "insttypes.h"

StatusCode translate_into_file(SymbolMap*, FILE*, FILE*);

int main(int argc, char **argv) {
    assert(argc > 2);

    // Open assembly file.
    FILE *file = fopen(argv[1], "r");
    // If file does not exist, it has not been opened successfully.
    if (!file) {
       return FILE_OPEN_ERROR;
    }

    // Completes first pass and rewinds file.
    SymbolMap* symbolMap = new_symbol_map(0);
    AssemblyInfo assemblyInfo = collect_symbols(symbolMap, file);

    FILE *outFile = fopen(argv[2], "w");
    StatusCode exitCode = translate_into_file(symbolMap, file, outFile);
    





    return EXIT_SUCCESS;
}

StatusCode translate_into_file(SymbolMap *symbolMap, FILE* file, FILE* outFile) {

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char* tokens[6] = { 0 };

    while ((read = getline(&line, &len, file)) != -1) {
        char *saveptr
        char *token = strtok_r(line, " ", saveptr);
        if (strstr(token, ":") != NULL) {

        }
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}
}