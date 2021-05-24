#ifndef __FETCHER_H__
#define __FETCHER_H__

#include "insttypes.h"

/**
 * Fetches next instruction from memory according to program counter.
 *
 * @param  state Machine state to decode the fetched instruction.
 * @return       Success state when decoding.
 */
StatusCode fetch(State *state);

#endif  // __FETCHER_H__
