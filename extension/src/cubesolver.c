#ifndef MAIN_IS_CALLING
#define MAIN_IS_CALLING
#endif

#include "solver/cubestate.h"
#include "solver/solver.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * The input file format:
 *
 * Line/Space delimited file.
 * All of A-F are substituted with a colour number, seen in cubestate.h
 *
 * A A A |
 * A A A |- TOP FACE
 * A A A |
 * B B B |
 * B B B |- FRONT FACE
 * B B B |
 * C C C |
 * C C C |- LEFT FACE
 * C C C |
 * D D D |
 * D D D |- BACK FACE
 * D D D |
 * E E E |
 * E E E |- RIGHT FACE
 * E E E |
 * F F F |
 * F F F |- BOTTOM FACE
 * F F F |
 */

/*
 * The output file format:
 *
 * Also a Line/Space delimited file.
 *
 * Motor ID (0-5, matches faces in cubestate.h)
 * | Direction to rotate (0 = CW, 1 = CCW)
 * | |
 * X Y
 */

bool load_in_file(const char *filename, CubeState *out_state);
bool export_solution(const char *filename, int move_count, Movement moves[static 20]);

int main(int argc, char **argv) {
    // Check argument count.
    if (argc != 3) {
        printf("Usage: cubesolver [infile] [outfile]");
        return 0;
    }

    // Load shuffled cube.
    CubeState main_state;
    load_in_file(argv[1], &main_state);

    // Solve shuffled cube.
    int total_moves = 0;
    Movement solution[20] = { { .face = TOP, .direction = CW } };
    solve(&main_state, &total_moves, solution);

    // Write output
    export_solution(argv[2], total_moves, solution);

    return 0;
}

// PRE: File is properly formatted.
bool load_in_file(const char *filename, CubeState *out_state) {
    // Open file.
    FILE *infile = fopen(filename, "r");
    if (!infile) {
        perror("Input file failed to open");
        return false;
    }

    // Clear cube state.
    out_state->history_count = 0;
    memset(out_state->history, 0, sizeof(out_state->history));

    // Load file into cube state.
    for (int i = 0; i < FACES * SIDE_LENGTH; ++i) {
        fscanf(infile, "%hhu %hhu %hhu\n",
            out_state->data[i / SIDE_LENGTH][i % SIDE_LENGTH],
            out_state->data[i / SIDE_LENGTH][i % SIDE_LENGTH] + 1,
            out_state->data[i / SIDE_LENGTH][i % SIDE_LENGTH] + 2
        );

        if (ferror(infile)) {
            perror("Failed to read from file");

            fclose(infile);
            return false;
        }
    }

    // Close file.
    fclose(infile);

    return true;
}

bool export_solution(const char *filename, int move_count, Movement moves[static 20]) {
    // Open file.
    FILE *outfile = fopen(filename, "w");
    if (!outfile) {
        perror("Output file failed to open");
        return false;
    }

    // Record all movements into file.
    for (int i = 0; i < move_count; ++i) {
        Movement *mv = moves + i;
        switch (mv->direction) {
            case CW:
                fprintf(outfile, "%hhu 0\n", mv->face);
                break;
            case DOUBLE:
                fprintf(outfile, "%hhu 0\n", mv->face);
                fprintf(outfile, "%hhu 0\n", mv->face);
                break;
            case CCW:
                fprintf(outfile, "%hhu 1\n", mv->face);
                break;
        }
    }

    // Close file.
    fclose(outfile);

    return false;
}

