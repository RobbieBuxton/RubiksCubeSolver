#ifndef __EXECUTEFUNCS_H__
#define __EXECUTEFUNCS_H__

#ifndef __SHORTEN__
#include "../../helpers/insttypes.h"
#else
#include "insttypes.h"
#endif

/**
 * Execute the next decoded Data Processing instruction.
 *
 * @param  state Current machine state
 * @return       Operation status code.
 */
StatusCode dp_execute(State *state);

/**
 * Execute the next decoded Multiply instruction.
 *
 * @param  state Current machine state
 * @return       Operation status code.
 */
StatusCode m_execute(State *state);

/**
 * Execute the next decoded Single Data Transfer instruction.
 *
 * @param  state Current machine state
 * @return       Operation status code.
 */
StatusCode sdt_execute(State *state);

/**
 * Execute the next decoded Branch instruction.
 *
 * @param  state Current machine state
 * @return       Operation status code.
 */
StatusCode b_execute(State *state);

/**
 * Halt the emulator.
 *
 * @param  state Current machine state
 * @return       HALT status code.
 */
StatusCode h_execute(State *state);

#endif  // __EXECUTEFUNCS_H__

