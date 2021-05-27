#ifndef __FETCHER_H__
#define __FETCHER_H__

#include "insttypes.h"

/**
 * Fetches next instruction from memory according to program counter.
 *
 * @param  state Machine state to fetch the fetched instruction for.
 * @return       Success state when fetching.
 */
StatusCode fetch(State *state);

#endif  // __FETCHER_H__

