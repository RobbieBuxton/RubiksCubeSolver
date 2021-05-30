#include "parser.h"

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

