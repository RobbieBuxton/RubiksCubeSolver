#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef __SHORTEN__
// Long include paths for language servers
#include "parser/symbols.h"
#include "parser/parser.h"
#include "translateinst/translate.h"
#include "../helpers/insttypes.h"
#include "../helpers/helpers.h"
#else
// Shortened includes
#include "symbols.h"
#include "parser.h"
#include "translate.h"
#include "insttypes.h"
#include "helpers.h"
#endif

StatusCode translate_into_file(SymbolMap *, FILE *, FILE *, AssemblyInfo);

int main(int argc, char **argv) {
    // Assert that we have an in file and an out file
    assert(argc > 3);

    // Open assembly file.
    FILE *file = fopen(argv[1], "r");
    // If file does not exist, it has not been opened successfully.
    if (!file) {
       return FILE_OPEN_ERROR;
    }

    // Completes first pass and rewinds file.
    SymbolMap* symbolMap = new_symbol_map(1);

    // Do we really need this assembly info anymore?
    AssemblyInfo assemblyInfo = collect_symbols(symbolMap, file);

    FILE *outFile = fopen(argv[2], "wb");
    StatusCode code = translate_into_file(symbolMap, file, outFile, assemblyInfo);

    fclose(outFile);
    fclose(file);

    free_symbol_map(symbolMap);

    status_code_handler(code, NULL);

    return EXIT_SUCCESS;
}

// function pointers to translate functions
static const TranslateFunction t_functions[5] = { dp_translate, m_translate, sdt_translate, b_translate, h_translate };

StatusCode translate_into_file(SymbolMap *symbolMap, FILE* file, FILE* outFile, AssemblyInfo assemblyInfo) {
    char line[MAXIMUM_LINE_LENGTH] = { '\0' };
    char* tokens[6] = { NULL };
    StatusCode code;
    uint offset = 0;

    // Read file line by line.
    while (!feof(file)) {
        // Read line
        if (!fgets(line, MAXIMUM_LINE_LENGTH, file)) {
            // Error while reading file.
            return FILE_READ_ERROR;
        }

        uint currentOp;
        char *savePtr;

        // Get first token from line
        char *token = strtok_r(line, " ", &savePtr);

        // If first token is a label, move onto next.
        if (strstr(token, ":") != NULL) {
            token = strtok_r(NULL, " ", &savePtr);
        }

        // Copy all tokens into array
        for (int i = 0; token != NULL; token = strtok_r(NULL, " ", &savePtr)) {
            tokens[i] = token;
        }

        // Call translate function from array according to the type represented by the first token.
        code = t_functions[type_from_string(tokens[0])](tokens, symbolMap, offset, &currentOp, assemblyInfo);

        // need error handling here

        // Write binary to file in little endian byte order.
        uint binary = swap_endianness(currentOp);
        fwrite(&binary, sizeof(binary), 1, outFile);

        // Increment offset so it holds the current line number of the file.
        offset++;
    }

    // Do we need to free anything else here?
    return code;
}
