#ifndef __DECODER_H__
#define __DECODER_H__

#include "insttypes.h"

/**
 * Decodes an instruction from the fetched field to the decoded field of a state.
 *
 * @param  state Machine state to decode the fetched instruction.
 * @return       Success state when decoding.
 */
StatusCode decode(State *state);

#endif  // __DECODER_H__
