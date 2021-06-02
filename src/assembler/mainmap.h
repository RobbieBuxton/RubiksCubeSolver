#ifndef __MAINMAP_H__
#define __MAINMAP_H__

#include "parser/symbols.h"

/**
 * A map containing a bunch of symbols used in translation.
 * Requires one-time initialisation in assemble.c, and freeing there at the end.
 *
 * Outside of assemble.c, do not add any symbols to this map, and do not free it.
 */
extern SymbolMap *translation_map;

#endif  // __MAINMAP_H__

