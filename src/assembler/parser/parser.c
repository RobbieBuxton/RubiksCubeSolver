#include "parser.h"
#include "symbols.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Reads and loads the entire assembly file into memory.
 * The returned buffer needs to be freed manually.
 *
 * @param  file_path Path to assembly file
 * @return           A string representing the contents of the whole file, or NULL if the read was unsuccessful.
 */
char *load_whole_file(const char *file_path) {
    // Open file and check if it exists.
    FILE *file = fopen(file_path, "r");

    if (!file) {
        perror("Error opening file: ");
        return NULL;
    }

    // Create a buffer for storing the file.
    char *file_buffer = NULL;

    // First: check the file length.
    if (fseek(file, 0, SEEK_END)) {
        goto size_err;
    }

    size_t size = ftell(file);
    if (!size) {
        goto size_err;
    }

    // Rewind the file stream
    if (fseek(file, 0, SEEK_SET)) {
        goto size_err;
    }

    goto size_ok;

size_err:
    // Print error, close file, return NULL;
    perror("Error while determining file size: ");

    fclose(file);
    return NULL;

size_ok:
    // Second: initialise the buffer.
    file_buffer = malloc(sizeof(char) * (size + 1));
    if (!file_buffer) {
        printf("Error occurred creating file buffer.\n");

        fclose(file);
        return NULL;
    }

    // Second-point-five: Null terminate the buffer.
    file_buffer[size] = '\0';

    // Third: read the whole file.
    size_t read = 0;
    if (!(read = fread(file_buffer, sizeof(char), size, file))) {
        perror("Error while reading file: ");

        fclose(file);
        free(file_buffer);

        return NULL;
    }

    // Four: assert that the whole file is read.
    if (read != size) {
        printf(
            "Error while reading file, possibly caused by early EOF. Expected %lu bytes, read %lu bytes instead.\n",
            read, size
        );

        fclose(file);
        free(file_buffer);

        return NULL;
    }

    // All good. Close the file and return the newly created buffer.
    fclose(file);
    return file_buffer;
}

/**
 * Find the first character in a string that is not whitespace.
 *
 * @param  start Where to start searching from
 * @return       A char * to the first non-whitespace, or NULL if not found
 */
char * first_non_whitespace(char *start) {
    // Whitespace characters to look for.
    static const char *whitespace = " \r\n\t";

    for (char *c = start; *c; ++c) {
        if (!strchr(whitespace, *c)) {
            return c;
        }
    }

    return NULL;
}

/**
 * First pass of the two-pass assembly, collecting the symbols in the file.
 *
 * @param  map           Symbol map to update
 * @param  file_contents The contents of the whole assembly file
 * @return               The number of symbols collected
 */
size_t collect_symbols(SymbolMap *map, char *file_contents) {
    // Line trackers
    char *line_start = file_contents;
    char temp_buf[MAXIMUM_SYMBOL_LENGTH] = { '\0' };

    // For keeping track of addresses
    uint lines_found = 0u;
    uint symbols_found = 0u;

    // Loop while the null terminator has not been reached.
    // File has at least one line, guaranteed by the file loader rejecting empty files.
    while (line_start) {
        // Trim off the leading whitespace.
        line_start = first_non_whitespace(line_start);

        // Look for the next line ending
        char *next_line = strchr(line_start, '\n');

        // Success flag for adding to map:
        bool r = true;

        if (next_line) {
            // Line exists, look one char before it.
            if (*(next_line - 1) == ':') {
                // Symbol found.
                size_t len = next_line - line_start;

                // We manually terminate the copied string with a null terminator.
                strncpy(temp_buf, line_start, len - 2);
                temp_buf[len - 1] = '\0';

                // For every line, that adds +4 to the byte offset of the file. We -4 for every symbol found.
                // E.g.
                //
                // mov r1,#1 <- @0d00
                // one:      <- @0d04
                // mov r2,#2 <- @0d04
                // mov r3,#3 <- @0d08
                // two:      <- @0d12
                // mov r4,#4 <- @0d12
                r = add_to_symbol_map(map, temp_buf, (lines_found * INSTRUCTION_WIDTH) - (++symbols_found * INSTRUCTION_WIDTH));
            }
        } else {
            // Line doesn't exist, look one char before null terminator.
            size_t len = strlen(line_start);
            if (line_start[len - 1] == ':') {
                // See above.
                strncpy(temp_buf, line_start, len - 2);
                temp_buf[len - 1] = '\0';

                r = add_to_symbol_map(map, temp_buf, (lines_found * INSTRUCTION_WIDTH) - (++symbols_found * INSTRUCTION_WIDTH));
            }
        }

        // If addition was unsuccessful, print an error.
        if (!r) {
            printf("Addition to map failed in first symbol collection pass!\n");
            break;
        }

        // Incrememnt line.
        ++lines_found;
        line_start = next_line + 1;
    }

    // Return the number of symbols found
    return symbols_found;
}

