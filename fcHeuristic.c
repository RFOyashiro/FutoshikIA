#include "fcHeuristic.h"

/**
 * All the variables affectatons
 */
AFFECTATION *affectationsFCH;

OLD_DOM **changedDomainFCH;

size_t affectedVarInd = 0;
AFFECTATION **affectedVar;

// All var that have other constraint than DIFF
size_t varWithDiffConstraintLastInd = 0;
AFFECTATION **varWithDiffConstraint;

// We can\'t use affectationsFCH to sort because of reference with the corresponding CASE
// (It reallys doesn\'t work with affectationsFCH)
AFFECTATION **sortedAffectations;

int nbNodesFCH = 1;
int nbConstraintTestedFCH = 0;

int *valuesUsage;

/**
 * Display an affectation for debug purpose
 * @param affect The affectationsFCH
 * @param lineSize The size of a line
 */
void displayAffectationFCH(AFFECTATION *affect, size_t lineSize) {

	CASE *caseToDisplay = affect->var;
	printf("Affectation for case : %zu\n", caseToDisplay->ind);
	printf("    current domain = {");
	int j;
	for (j = 0; j < lineSize; ++j) {
		printf("%d", affect->curDomain[j]);
		if (j < lineSize - 1)
			printf(", ");
	}
	printf("}\n");
	printf("	current domain size = %zu\n", affect->curDomainSize);
	printf("	current Value = %d\n", affect->curValue);
	printf("	compt = %d\n\n", affect->compt);

}

void freeFCH(CASE *grid, size_t lineSize, int varHeuristicSmallestDomainSize, int diffConstraint, int valueheuristic) {
	size_t i;

	for (i = 0; i < lineSize * lineSize; ++i) {
		free(changedDomainFCH[i]);
	}
	free(changedDomainFCH);

	for (i = 0; i < lineSize * lineSize; ++i) {
		(&grid[i])->affectation = NULL;
		AFFECTATION *curAff = &affectationsFCH[i];
		free(curAff->curDomain);
		free(curAff->previousDomain);
	}
	free(affectationsFCH);
	free(affectedVar);
	
	if (diffConstraint) {
		free(varWithDiffConstraint);
	}
	if(varHeuristicSmallestDomainSize) {
		free(sortedAffectations);
	}
	
	// This free cause some wierd free errors
	/*if (valueheuristic) {
		free(valuesUsage);
	}*/
}

void changeDomainFCH(int oldValue, size_t indice, AFFECTATION *origin, AFFECTATION *modified) {
	OLD_DOM old;
	old.oldDomainValue = oldValue;
	old.indice = indice;
	old.origin = origin;
	changedDomainFCH[modified->var->ind][modified->compt++] = old;
	if (DEBUG_FCH)
		printf("removed %d from var %zu\n", oldValue, origin->var->ind);
	origin->curDomain[indice] = NO_DOMAINE;
	origin->curDomainSize--;
}

int checkConstraintFCH(CONTRAINTE *constraint, AFFECTATION * curAff, size_t lineSize) {

	// Must only modify instancied var
	if (constraint->gauche->affectation->curValue != NO_DOMAINE &&
			constraint->droite->affectation->curValue != NO_DOMAINE)
		return 1;

	int i;
	switch (constraint->op) {
		case DIF:
			if (constraint->gauche->affectation == curAff) {
				for (i = 0; i < lineSize; i++) {
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE &&
							constraint->droite->affectation->curDomain[i] == curAff->curValue) {

						changeDomainFCH(constraint->droite->affectation->curDomain[i],
								i, constraint->droite->affectation, curAff);
					}
				}

				if (constraint->droite->affectation->curDomainSize > 0)
					return 1;
				else {
					return 0;
				}
			}
			else {
				for (i = 0; i < lineSize; i++) {
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE &&
							constraint->gauche->affectation->curDomain[i] == curAff->curValue) {

						changeDomainFCH(constraint->gauche->affectation->curDomain[i],
								i, constraint->gauche->affectation, curAff);
					}
				}

				if (constraint->gauche->affectation->curDomainSize > 0)
					return 1;
				else {
					return 0;
				}
			}
			break;



		case SUP:
			if (constraint->gauche->affectation == curAff) {
				for (i = 0; i < lineSize; i++) {
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE &&
							!(curAff->curValue > constraint->droite->affectation->curDomain[i])) {

						changeDomainFCH(constraint->droite->affectation->curDomain[i],
								i, constraint->droite->affectation, curAff);
					}
				}


				if (constraint->droite->affectation->curDomainSize > 0)
					return 1;
				else
					return 0;
			}
			else {
				for (i = 0; i < lineSize; i++) {
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE &&
							!(constraint->gauche->affectation->curDomain[i] > curAff->curValue)) {

						changeDomainFCH(constraint->gauche->affectation->curDomain[i],
								i, constraint->gauche->affectation, curAff);
					}
				}

				if (constraint->gauche->affectation->curDomainSize > 0)
					return 1;
				else
					return 0;
			}
			break;




		case INF:
			if (constraint->gauche->affectation == curAff) {
				for (i = 0; i < lineSize; i++) {
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE &&
							!(curAff->curValue < constraint->droite->affectation->curDomain[i])) {

						changeDomainFCH(constraint->droite->affectation->curDomain[i],
								i, constraint->droite->affectation, curAff);
					}
				}

				if (constraint->droite->affectation->curDomainSize > 0)
					return 1;
				else
					return 0;
			}
			else {
				for (i = 0; i < lineSize; i++) {
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE &&
							!(constraint->gauche->affectation->curDomain[i] < curAff->curValue)) {

						changeDomainFCH(constraint->gauche->affectation->curDomain[i],
								i, constraint->gauche->affectation, curAff);
					}
				}

				if (constraint->gauche->affectation->curDomainSize > 0)
					return 1;
				else
					return 0;
			}
			break;
		default:
			return ERROR;
	}
}

void rewriteDomainFCH(AFFECTATION * curAff, size_t lineSize) {

	if (DEBUG_FCH)
		printf("backing %zu\n", curAff->var->ind);
	int i;
	for (i = 0; i < curAff->compt; i++) {
		OLD_DOM *old = &changedDomainFCH[curAff->var->ind][i];
		old->origin->curDomain[old->indice] = old->oldDomainValue;
		old->origin->curDomainSize++;
		if (DEBUG_FCH)
			printf("adding %d to var %zu\n", old->oldDomainValue, old->origin->var->ind);
	}
	curAff->compt = 0;
}

/**
 * Find the least use value within a var domain, return -1 if domain empty
 */
int findLeastUseValue(AFFECTATION *curAff, size_t lineSize) {
	size_t j;
	size_t leastUseValUsage = lineSize * lineSize;
	ssize_t leastUseValInd = -1;
	
	for (j = 0; j < lineSize; ++j) {
		if(curAff->curDomain[j] != NO_DOMAINE && valuesUsage[j] < leastUseValUsage) {
			leastUseValInd = j;
			leastUseValUsage = valuesUsage[j];
		}
	}
	return leastUseValInd;
}

int chooseNextValueNoH(AFFECTATION *curAff, size_t lineSize, CONTRAINTE *contraintes) {
	int consistant = 0;
	size_t j, k;
	CONTRAINTE *curConst;
	for (j = 0; j < lineSize && !consistant; ++j) {

		consistant = 0;
		if (curAff->curDomain[j] != NO_DOMAINE) {

			curAff->curValue = curAff->curDomain[j];
			curAff->curDomain[j] = NO_DOMAINE;
			curAff->curDomainSize--;

			// Check constraint
			consistant = 1;
			for (k = 0; k < curAff->var->indLastConst && consistant; ++k) {
				curConst = &contraintes[curAff->var->conts[k]];

				nbConstraintTestedFCH++;
				
				if (!checkConstraintFCH(curConst, curAff, lineSize)) {
					consistant = 0;
					// If the affectation wasn't concistant, we rollback what we did
					rewriteDomainFCH(curAff, lineSize);
					if (DEBUG_FCH) {
						printf("failed for %d: \n", curAff->curValue);
						displayAffectationFCH(curAff, lineSize);
					}
				}
			}
		}
	}
	return consistant;
}

/*
 * The heuristic is just to choose the least use value
 */
int chooseNextValue(AFFECTATION *curAff, size_t lineSize, CONTRAINTE *contraintes, int valueheuristic) {

	if (!valueheuristic) {
		return chooseNextValueNoH(curAff, lineSize, contraintes);
	}
	
	int consistant = 0;
	ssize_t leastUseVal;
	size_t k;
	CONTRAINTE *curConst;
	
	leastUseVal = findLeastUseValue(curAff, lineSize);
	while(!consistant && leastUseVal > -1) {
		
		curAff->curValue = curAff->curDomain[leastUseVal];
		curAff->curDomain[leastUseVal] = NO_DOMAINE;
		curAff->curDomainSize--;
		
		//printf("affected \n");
		//displayAffectationFCH(curAff, lineSize);

		// Check constraint
		consistant = 1;
		for (k = 0; k < curAff->var->indLastConst && consistant; ++k) {
			curConst = &contraintes[curAff->var->conts[k]];
			
			// Increment the number of tested constraint
			nbConstraintTestedFCH++;

			if (!checkConstraintFCH(curConst, curAff, lineSize)) {
				consistant = 0;
				// If the affectation wasn't concistant, we rollback what we did
				rewriteDomainFCH(curAff, lineSize);
				if (DEBUG_FCH) {
					printf("failed for %d: \n", curAff->curValue);
					displayAffectationFCH(curAff, lineSize);
				}
			}
		}
		
		leastUseVal = findLeastUseValue(curAff, lineSize);
	}
	//displayAffectationFCH(curAff, lineSize);
	//printf("consistant %i\n", consistant);
	return consistant;
}

AFFECTATION *chooseNextVarNoH(size_t lineSize) {
	
	size_t i;
	AFFECTATION *curAff = NULL;
	
	for (i = 0; i < lineSize * lineSize; ++i) {
		
		if((&affectationsFCH[i])->curValue == NO_DOMAINE) {
			curAff = &affectationsFCH[i];
			break;
		}
	}
	return curAff;
}
/*
 * The heuristics we use :
 * - First we search for smallest valid domain size if it is < lineSize / 2 (affecationsFCH is sorted)
 * - Then we search for var with constraint other than '!=' ('<' or '>')
 * - Finally we take the var with smallest valid domain size above lineSize / 2
 */
AFFECTATION *chooseNextVar(size_t lineSize, int varHeuristicSmallestDomainSize, int diffConstraint) {
	
	// If no heuristic should be applied
	if (!varHeuristicSmallestDomainSize && !diffConstraint) {
		return chooseNextVarNoH(lineSize);
	}

	AFFECTATION *curAff = NULL;
	size_t i, j;

	if (varHeuristicSmallestDomainSize) {
		// Choose next var which is the one with the smallest domain size wich is < lineSize / 2
		for (i = 0; i < lineSize * lineSize; ++i) {

			if (sortedAffectations[i]->curValue == NO_DOMAINE) {
				// If the current valid domain size is > to lineSize / 2 looks for an other heuristic
				if (sortedAffectations[i]->curDomainSize > lineSize / 2) {
					break;
				}
				curAff = sortedAffectations[i];
				return curAff;
			}
		}
	}

	if (diffConstraint) {
		// Choose a var which has a constraint other than DIF
		for (j = 0; j < varWithDiffConstraintLastInd; ++j) {
			if (varWithDiffConstraint[j]->curValue == NO_DOMAINE) {
				curAff = varWithDiffConstraint[j];
				return curAff;
			}
		}
	}

	if (varHeuristicSmallestDomainSize) {
		// Choose next var which is the one with the smallest domain size
		for (; i < lineSize * lineSize; ++i) {
			if (sortedAffectations[i]->curValue == NO_DOMAINE) {
				curAff = sortedAffectations[i];
				return curAff;
			}
		}
	}
	// If the default heuristic shoudn't be used, use default process
	else {
		return chooseNextVarNoH(lineSize);
	}

	return curAff;
}

int fcHeuritic(CASE *grid, size_t lineSize,
		CONTRAINTE *contraintes, size_t nbContraintes, int varHeuristicSmallestDomainSize, 
		int diffConstraint, int valueheuristic) {

	if (nbContraintes == 0 || contraintes == NULL || grid == NULL ||
			lineSize == 0) {
		return ERROR;
	}

	size_t i, j;
	affectationsFCH = malloc(lineSize * lineSize * sizeof (AFFECTATION));
	changedDomainFCH = malloc(lineSize * lineSize * sizeof (OLD_DOM*));
	affectedVar = malloc(lineSize * lineSize * sizeof (AFFECTATION*));
	
	if(diffConstraint) {
		varWithDiffConstraint = malloc(lineSize * lineSize * sizeof (AFFECTATION*));
	}
	
	if (valueheuristic) {
		valuesUsage = malloc(lineSize * sizeof (int));
		
		for (i = 0; i < lineSize; ++i) {
			valuesUsage[i] = 0;
		}
	}
	
	if(varHeuristicSmallestDomainSize) {
		sortedAffectations = malloc(lineSize * lineSize * sizeof (AFFECTATION*));
	}

	CONTRAINTE *curConst;
	CASE *curCase;

	for (i = 0; i < lineSize * lineSize; ++i) {
		AFFECTATION *curAff = &affectationsFCH[i];
		
		curCase = &grid[i];
		curAff->var = curCase;
		curAff->curDomain = malloc(lineSize * sizeof (int));
		memmove(curAff->curDomain, curCase->domaine, lineSize * sizeof (int));

		// If there is a domain constraint
		if (curAff->curDomain[0] == NO_DOMAINE || (lineSize > 1 && curAff->curDomain[1] == NO_DOMAINE)) {
			curAff->curDomainSize = 1;
		}
		else {
			curAff->curDomainSize = lineSize;
		}

		curAff->previousDomain = malloc(lineSize * sizeof (int));
		curAff->curValue = NO_DOMAINE;
		curAff->compt = 0;
		curCase->affectation = curAff;

		changedDomainFCH[i] = malloc(curAff->var->indLastConst * lineSize * sizeof (OLD_DOM));

		// Test if there is a constraint other than DIF
		for (j = 0; diffConstraint && j < curAff->var->indLastConst; ++j) {
			curConst = &contraintes[curAff->var->conts[j]];
			if (curConst->op != DIF) {
				varWithDiffConstraint[varWithDiffConstraintLastInd] = curAff;
				varWithDiffConstraintLastInd++;
				break;
			}
		}
		
		if(varHeuristicSmallestDomainSize) {
			sortedAffectations[i] = curAff;
		}
	}
	
	if(varHeuristicSmallestDomainSize) {
		// Sort by valid domain size so that smallest valid domain size is in first 
		// (regardless of the current value)
		quickSortMain(sortedAffectations, lineSize * lineSize);
	}
	/*for (i = 0; i < lineSize * lineSize; ++i) {
		printf("var %zu\n", sortedAffectations[i]->var->ind);
	}*/

	if (DEBUG_FCH)
		printf("Initialization completed. Beginning main loop\n");

	// Main loop
	int success = 1;
	int consistant = 1;
	
	AFFECTATION *curAff;
	curAff = chooseNextVar(lineSize, varHeuristicSmallestDomainSize, diffConstraint);
	
	size_t k;
	while (curAff != NULL) {

		if (DEBUG_FCH) {
			printf("var : %zu\n", curAff->var->ind);
		}

		// Affectation
		consistant = 0;

		if (DEBUG_FCH) {
			displayAffectationFCH(curAff, lineSize);
		}
		
		consistant = chooseNextValue(curAff, lineSize, contraintes, valueheuristic);
		if (consistant) {

			affectedVar[affectedVarInd] = curAff;
			affectedVarInd++;

			if (valueheuristic) {
				// increment of the value frequency
				valuesUsage[curAff->curValue -1]++;
			}

			// If we go deeper, we need to save the next var domain
			// So when it goes up, it can undo the current affectation tested
			AFFECTATION *nextAff = chooseNextVar(lineSize, varHeuristicSmallestDomainSize, diffConstraint);
			if (nextAff != NULL) {
				// We save the current domain
				memmove(nextAff->previousDomain, nextAff->curDomain,
						lineSize * sizeof (int));
			}

			if (DEBUG_FCH)
				printf("consistant pour %zu avec la valeur %d\n",
					curAff->var->ind, curAff->curValue);

			if(varHeuristicSmallestDomainSize) {
				// Sort by valid domain size so that smallest valid domain size is in first 
				// (regardless of the current value)
				quickSortMain(sortedAffectations, lineSize * lineSize);
			}
			
			// Increment the number of node in the tree
			nbNodesFCH++;
		}		
		// if we didn't found a concistant affectation
		if (!consistant) {
			// If we backtracked to root and still not consistant then we failed
			if (affectedVarInd == 0) {
				success = 0;
				break;
			}
			// otherwise we go up
			else {
				if (DEBUG_FCH) {
					printf("failure for %zu with value %d\n", curAff->var->ind, curAff->curValue);
				}

				curAff->curValue = NO_DOMAINE;
				

				// When we go up, we need to undo the values that we just have tested without changing what
				// other affectation have made
				memmove(curAff->curDomain, curAff->previousDomain, lineSize * sizeof (int));
				// Recalculate the valid domain size
				curAff->curDomainSize = 0;
				for (k = 0; k < lineSize; ++k) {
					if (curAff->curDomain[k] != NO_DOMAINE) {
						curAff->curDomainSize++;
					}
				}

				// We also need to rewrite things that have been to change by previous var before 
				// taking another value
				AFFECTATION *previous = affectedVar[affectedVarInd - 1];
				rewriteDomainFCH(previous, lineSize);
				
				if (valueheuristic) {
					// decrement of the value frequency
					valuesUsage[previous->curValue - 1]--;
				}

				// Going up
				previous->curValue = NO_DOMAINE;
				affectedVarInd--;
				

				if (DEBUG_FCH) {
					printf("now backing to %zu\n", previous->var->ind);
				}
			}
		}
		curAff = chooseNextVar(lineSize, varHeuristicSmallestDomainSize, diffConstraint);
	}

	if (success) {
		printf("Solution is :\n");
		for (i = 0; i < lineSize * lineSize; ++i) {

			AFFECTATION * curAffToDisplay = &affectationsFCH[i];
			printf("%d", curAffToDisplay->curValue);
			if ((i + 1) % lineSize == 0)
				printf("\n");
			else
				printf(" ");
		}
	}
	else {
		printf("Failure : no solution\n");
	}
	
	printf("Node in the corresponding tree : %d\n", nbNodesFCH);
	printf("Number of constraint test : %d\n", nbConstraintTestedFCH);

	if (DEBUG_FCH) {
		printf("Freeing\n");
	}
	freeFCH(grid, lineSize, varHeuristicSmallestDomainSize, diffConstraint, valueheuristic);
	return success;
}
