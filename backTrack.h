/*
 * Warning, use this only after the grid has been initialized 
 */
#ifndef BACKTRACK_C
#define BACKTRACK_C

#include "define.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_BACKTRACK 1

int backTrack(CASE *grid, size_t lineSize, 
	CONTRAINTE *contraintes, size_t nbContraintes);

#endif
