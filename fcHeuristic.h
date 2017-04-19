#ifndef __FCH_H__
#define __FCH_H__

#include "define.h"
#include "quickSort.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_FCH 0

/**
 * Apply forward checking algorithim to solve futoshiki grid
 * @param grid  The grid to solve
 * @param lineSize The size of one line in the grid
 * @param contraintes The constraints of the gride var
 * @param nbContraintes The number of constraint of the grid var
 * @param varHeuristicSmallestDomainSize Should we apply smallest domain size heuristic.
 * @param diffConstraint Should we apply constraint other than DIF heuristic.
 * @param valueheuristic Should we apply value heuristic.
 * @return 1 if solution found, 0 otherwise
 */
int fcHeuritic(CASE *grid, size_t lineSize,
		CONTRAINTE *contraintes, size_t nbContraintes, int varHeuristicSmallestDomainSize, 
		int diffConstraint, int valueheuristic);

#endif
