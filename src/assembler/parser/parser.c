#include "parser.h"
#include "symbols.h"

#ifndef __SHORTEN__
// See symbols.h for information about this conditional macro.
#include "../../helpers/helpers.h"
#else
#include "helpers.h"
#endif

#include <stddef.h>
#include <string.h>

AssemblyInfo collect_symbols(SymbolMap *map, FILE *file) {
    // Line trackers
    char line_buf[MAXIMUM_LINE_LENGTH] = { '\0' };
    char temp_buf[MAXIMUM_SYMBOL_LENGTH] = { '\0' };

    // Info on collected instructions and symbols.
    AssemblyInfo file_info = { 0, 0 };

    // For keeping track of addresses
    uint lines_found = 0u;
    uint symbols_found = 0u;

    // Keep reading until we reach EOF
    while (!feof(file)) {
        // Read a line from the file.
        if (!fgets(line_buf, MAXIMUM_LINE_LENGTH, file)) {
            // Error while reading file.
            perror("Error while reading file: ");

            rewind(file);
            return file_info;
        }

        // Find first non-whitespace character.
        char *first_char = first_non_whitespace(line_buf);

        if (first_char) {
            ++lines_found;

            // There is a line here, start parsing it.
            size_t len = strlen(first_char);

            if (first_char[len - 1] == ':') {
                // This is a symbol.
                strcpy(temp_buf, first_char);

                // Remove the :
                temp_buf[len - 2] = '\0';

                // Add to symbol map
                add_to_symbol_map(
                    map, temp_buf,
                    (lines_found  - (++symbols_found)) * INSTRUCTION_WIDTH
                );
            }
        }
    }

    // Rewind the file stream for the second pass.
    rewind(file);

    // Update file info struct
    file_info.symbols = symbols_found;
    file_info.instructions = lines_found - symbols_found;

    return file_info;
}

