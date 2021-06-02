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

#include "mainmap.h"

StatusCode translate_into_file(SymbolMap *, FILE *, FILE *, AssemblyInfo *);
void init_translation_map(void);

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
    AssemblyInfo assemblyInfo = collect_symbols(symbolMap, file);

    // Creates new file and pads it for load immediate instructions
    FILE *outFile = fopen(argv[2], "wb+");
    for( ;ftell(outFile) < assemblyInfo.instructions * 4; ) {
        putc(0u, outFile);
    }
    rewind(outFile);
    StatusCode code = translate_into_file(symbolMap, file, outFile, &assemblyInfo);

    fclose(outFile);
    fclose(file);

    free_symbol_map(symbolMap);
    free_symbol_map(translation_map);

    status_code_handler(code, NULL);

    return EXIT_SUCCESS;
}

// Declaration of translation_map for mainmap.h
SymbolMap *translation_map = NULL;

void init_translation_map(void) {
    // Small initial size. It will self-extend later.
    translation_map = new_symbol_map(4);

    // TODO: Add in the symbols.
}

// function pointers to translate functions
static const TranslateFunction t_functions[5] = { dp_translate, m_translate, sdt_translate, b_translate, h_translate };

StatusCode translate_into_file(SymbolMap *symbolMap, FILE* file, FILE* outFile, AssemblyInfo *assemblyInfo) {
    char line[MAXIMUM_LINE_LENGTH] = { '\0' };
    StatusCode code;
    uint offset = 0;

    // Read file line by line.
    while (!feof(file)) {
        // tokens set to null in each iteration
        char* tokens[6] = { NULL };
        // Read line
        if (!fgets(line, MAXIMUM_LINE_LENGTH, file)) {
            // Error while reading file.
            return FILE_READ_ERROR;
        }

        // Remove the newline left behind by fgets (if it is there).
        char *newl = strchr(line, '\n');
        if (newl) {
            *newl = '\0';
        }

        uint currentOp;
        char *savePtr;

        // Get first token from line
        char *token = strtok_r(line, " ,", &savePtr);

        // If first token is a label, move onto next.
        if (strstr(token, ":") != NULL) {
            token = strtok_r(NULL, " ,", &savePtr);
        }

        // Copy all tokens into array
        for (int i = 0; token != NULL; token = strtok_r(NULL, " ,", &savePtr)) {
            tokens[i] = token;
        }

        // Call translate function from array according to the type represented by the first token.
        code = t_functions[type_from_string(tokens[0])](tokens, symbolMap, offset, &currentOp, assemblyInfo);

        // need error handling here

        // Write binary to file in little endian byte order.
        uint binary = swap_endianness(currentOp);
        fwrite(&binary, sizeof(binary), 1, outFile);

        // If there was a ldr instruction which required a value added to the binary file.
        if (assemblyInfo->int_to_load) {
            // Store the current position of the file.
            fpos_t *nextOp;
            fgetpos(outFile, nextOp);
            // Seek the end of the file and save the value there.
            fseek(outFile, assemblyInfo->instructions * 4, SEEK_SET);
            binary = swap_endianness(assemblyInfo->load_int);
            fwrite(&binary, sizeof(binary), 1, outFile);
            // Revert the file pointer to the correct place.
            fsetpos(outFile, nextOp);

            // Update assembly info so the next value is saved in the next space
            assemblyInfo->int_to_load = false;
            assemblyInfo->instructions++;
        }

        // Increment offset so it holds the current line number of the file.
        offset++;
    }

    // Do we need to free anything else here?
    return code;
}
