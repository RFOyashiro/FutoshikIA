#ifndef __FCH_H__
#define __FCH_H__

#include "define.h"
#include "quickSort.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_FCH 0

int fcHeuritic(CASE *grid, size_t lineSize,
		CONTRAINTE *contraintes, size_t nbContraintes, int varHeuristicSmallestDomainSize, 
		int diffConstraint, int valueheuristic);

#endif
