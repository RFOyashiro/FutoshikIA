#ifndef __FOR_H__
#define __FOR_H__

#include "define.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_FC 0

/**
 * Apply forward checking algorithim to solve futoshiki grid
 * @param grid  The grid to solve
 * @param lineSize The size of one line in the grid
 * @param contraintes The constraints of the gride var
 * @param nbContraintes The number of constraint of the grid var
 * @return 1 if solution found, 0 otherwise
 */
int forwardChecking(CASE *grid, size_t lineSize, 
	CONTRAINTE *contraintes, size_t nbContraintes);

#endif
