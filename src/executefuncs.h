#ifndef __EXECUTEFUNCS_H__
#define __EXECUTEFUNCS_H__

#include "insttypes.h"

/**
 * Execute the next decoded Data Processing instruction.
 *
 * @param  state Current machine state
 * @return       Operation success code.
 */
StatusCode dp_execute(State* state);

/**
 * Execute the next decoded Multiply instruction.
 *
 * @param  state Current machine state
 * @return       Operation success code.
 */
StatusCode m_execute(State* state);

/**
 * Execute the next decoded Single Data Transfer instruction.
 *
 * @param  state Current machine state
 * @return       Operation success code.
 */
StatusCode sdt_execute(State* state);

/**
 * Execute the next decoded Branch instruction.
 *
 * @param  state Current machine state
 * @return       Operation success code.
 */
StatusCode b_execute(State* state);

#endif  // __EXECUTEFUNCS_H__
