#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser/symbols.h"
#include "parser/parser.h"
#include "translateinst/translate.h"
#include "../helpers/helpers.h"

#include "mainmap.h"

StatusCode translate_into_in_file(SymbolMap *, FILE *, FILE *, AssemblyInfo *);
void init_translation_map(void);

int main(int argc, char **argv) {
    // Assert that we have an in file and an out file
    assert(argc >= 3);

    // Initialise translation map
    init_translation_map();

    // Open assembly file.
    FILE *in_file = fopen(argv[1], "r");
    // If in_file does not exist, it has not been opened successfully.
    if (!in_file) {
       return FILE_OPEN_ERROR;
    }

    // Completes first pass and rewinds file.
    SymbolMap* symbol_map = new_symbol_map(1);
    AssemblyInfo assembly_info = collect_symbols(symbol_map, in_file);

    // Creates new file and pads it for load immediate instructions
    FILE *out_file = fopen(argv[2], "wb+");

    // Translate assembly into output file.
    StatusCode code = translate_into_in_file(symbol_map, in_file, out_file, &assembly_info);

    fclose(out_file);
    fclose(in_file);

    free_symbol_map(symbol_map);
    free_symbol_map(translation_map);

    status_code_handler(code, NULL);

    return EXIT_SUCCESS;
}

// Declaration of translation_map for mainmap.h
SymbolMap *translation_map = NULL;

void init_translation_map(void) {
    // Small initial size. It will self-extend later.
    translation_map = new_symbol_map(4);
    assert(translation_map);

    // Data Processing
    add_to_symbol_map(translation_map, "and", dp_and);
    add_to_symbol_map(translation_map, "eor", dp_eor);
    add_to_symbol_map(translation_map, "sub", dp_sub);
    add_to_symbol_map(translation_map, "rsb", dp_rsb);
    add_to_symbol_map(translation_map, "add", dp_add);
    add_to_symbol_map(translation_map, "orr", dp_orr);
    add_to_symbol_map(translation_map, "mov", dp_mov);
    add_to_symbol_map(translation_map, "tst", dp_tst);
    add_to_symbol_map(translation_map, "teq", dp_teq);
    add_to_symbol_map(translation_map, "cmp", dp_cmp);
    add_to_symbol_map(translation_map, "andeq", dp_andeq);
    add_to_symbol_map(translation_map, "lsl", dp_lsl);

    // Branch
    add_to_symbol_map(translation_map, "beq", eq);
    add_to_symbol_map(translation_map, "bne", ne);
    add_to_symbol_map(translation_map, "bge", ge);
    add_to_symbol_map(translation_map, "blt", lt);
    add_to_symbol_map(translation_map, "bgt", gt);
    add_to_symbol_map(translation_map, "ble", le);
    add_to_symbol_map(translation_map, "bal", al);
    add_to_symbol_map(translation_map, "b",   al);

    // Multiply
    add_to_symbol_map(translation_map, "mul", mul);
    add_to_symbol_map(translation_map, "mla", mla);

    // Single Data Transfer
    add_to_symbol_map(translation_map, "ldr", ldr);
    add_to_symbol_map(translation_map, "str", str);
}

// function pointers to translate functions
static const TranslateFunction t_functions[5] = { dp_translate, m_translate, sdt_translate, b_translate, h_translate };

// Endianness check (because we need to ensure that the file is *definitely* in little endian).
// Returns true if big endian.
bool check_endianness(void) {
    // Somewhat undefined behaviour is being used here...
    union {
        uint32_t i;
        uchar buf[4];
    } checker;

    checker.i = 1;
    return checker.buf[3];
}

StatusCode translate_into_in_file(SymbolMap *symbol_map,FILE* in_file, FILE* out_file, AssemblyInfo *assembly_info) {
    char line[MAXIMUM_LINE_LENGTH] = { '\0' };
    StatusCode code;
    uint offset = 0;
    bool big_e = check_endianness();

    // Read in_file line by line.
    while (!feof(in_file)) {
        // tokens set to null in each iteration
        char *tokens[6] = { NULL };

        // Read line
        if (!fgets(line, MAXIMUM_LINE_LENGTH, in_file)) {
            if (errno) {
                // Error while reading file.
                return FILE_READ_ERROR;
            } else {
                return CONTINUE;
            }
        }

        // Strip leading whitespace
        char *true_start = first_non_whitespace(line);

        if (true_start) {
            // Remove the newline left behind by fgets (if it is there).
            char *nl = strchr(line, '\n');
            if (nl) {
                *nl = '\0';
            }

            uint current_op = 0u;
            char *save_ptr;

            // Get first token from line
            char *token = strtok_r(true_start, " ,", &save_ptr);

            // If first token is a label, move onto next line.
            if (strstr(token, ":") != NULL) {
                // token = strtok_r(NULL, " ,", &save_ptr);
                continue;
            }

            // Copy all tokens into array
            for (int i = 0; token != NULL; token = strtok_r(NULL, " ,", &save_ptr)) {
                tokens[i++] = token;
            }

            // Call translate function from array according to the type represented by the first token.
            code = t_functions[type_from_string(tokens[0])](tokens, symbol_map, offset, &current_op, assembly_info);

            // Error handling here
            if (code) {
                fprintf(stderr, "Error! See code for details: Code %d\n", code);
            }

            // Write binary to file in little endian byte order.
            uint binary = big_e ? swap_endianness(current_op) : current_op;
            fwrite(&binary, sizeof(uint), 1, out_file);

            // If there was a ldr instruction which required a value added to the binary file.
            if (assembly_info->int_to_load) {
                // Store the current position of the file.
                fpos_t nextOp;
                fgetpos(out_file, &nextOp);

                // Seek the end of the file and save the value there.
                fseek(out_file, assembly_info->instructions * INSTRUCTION_BYTE_LENGTH, SEEK_SET);
                binary = big_e ? swap_endianness(assembly_info->load_int) : assembly_info->load_int;
                fwrite(&binary, sizeof(binary), 1, out_file);

                // Revert the file pointer to the correct place.
                fsetpos(out_file, &nextOp);

                // Update assembly info so the next value is saved in the next space
                assembly_info->int_to_load = false;
                assembly_info->instructions++;
            }

            // Increment offset so it holds the current line number of the file.
            offset++;
        }
    }

    // Do we need to free anything else here?
    return code;
}

StatusCode h_translate(char **tokens, SymbolMap *symbols, uint current_offset, uint *output, AssemblyInfo *assembly_info) {
    *output = 0u;
    return CONTINUE;
}

