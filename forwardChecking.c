#include "forwardChecking.h"

/**
 * All the variables affectatons
 */
AFFECTATION *affectations;

/**
 * The matrix of changed domain. It's to remember all domain modifications to cancel them in case of failure.
 * The first level array size is lineSize * lineSize, one sub array for each var. In each sub array we create an OLD_DOM
 * variable when this var makes a domain modification.
 */
OLD_DOM **changedDomain;

// The number of nodes in the corresponding tree
int nbNodesFC = 1;
// The number of constraint tested
int nbConstraintTestedFC = 0;

/**
 * Display an affectation for debug purpose
 * @param affect The affectations
 * @param lineSize The size of a line
 */
void displayAffectationFC(AFFECTATION *affect, size_t lineSize) {

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
	printf("	current Value = %d\n", affect->curValue);
	printf("	compt = %d\n\n", affect->compt);

}

/**
 * Free all malloc things and reset everything.
 * @param grid The grid (to reser affectation)
 * @param lineSize The size of one line in the grid
 */
void freeFC(CASE *grid, size_t lineSize) {
	size_t i;
	
	// Free old domains
	for (i = 0; i < lineSize * lineSize; ++i) {
		free(changedDomain[i]);
	}
	free(changedDomain);
	
	// Free affectations
	for (i = 0; i < lineSize * lineSize; ++i) {
		(&grid[i])->affectation = NULL;
		AFFECTATION *curAff = &affectations[i];
		free(curAff->curDomain);
		free(curAff->previousDomain);
	}
	free(affectations);
}

/**
 * Replace a value in a domain with NO_DOMAINE
 * @param oldValue The old value.
 * @param indice The index of the value in the domain.
 * @param origin The modified affectation.
 * @param modified The modifier affectation.
 */
void changeDomain(int oldValue, size_t indice, AFFECTATION *origin, AFFECTATION *modifier) {
	OLD_DOM old;
	old.oldDomainValue = oldValue;
	old.indice = indice;
	old.origin = origin;
	changedDomain[modifier->var->ind][modifier->compt++] = old;
	if (DEBUG_FC)
		printf("removed %d from var %zu\n", oldValue, origin->var->ind);
	origin->curDomain[indice] = NO_DOMAINE;
	origin->curDomainSize--;
}

/**
 * Apply a constraint and check if it is consistant.
 * @param constraint The constraint to test.
 * @param curAff  The current affectation.
 * @param lineSize The size of one line in the grid.
 * @return 1 if consistant, 0 otherwise.
 */
int checkConstraintFC(CONTRAINTE *constraint, AFFECTATION * curAff, size_t lineSize) {
	
	// Must only modify instancied var
	if (constraint->gauche->affectation->curValue != NO_DOMAINE && 
			constraint->droite->affectation->curValue != NO_DOMAINE)
		return 1;
	
	int i;
	// Check constraint type
	switch (constraint->op) {	
		case DIF:
			// Check if we need to apllay modifications on left or right operand
			if (constraint->gauche->affectation == curAff) {
				// For each value of the domain
				for (i = 0; i < lineSize; i++){
					// Look for the value to modifiy
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE && 
							constraint->droite->affectation->curDomain[i] == curAff->curValue) {
						
						// Change the domain
						changeDomain(constraint->droite->affectation->curDomain[i], 
								i, constraint->droite->affectation, curAff);
					}
				}
				
				// The is still a valid value in the domain then the affectation is consistant with this constraint
				if (constraint->droite->affectation->curDomainSize > 0) 
					return 1;
				else 
					return 0;
			}
			else {
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE && 
							constraint->gauche->affectation->curDomain[i] == curAff->curValue) {
						
						changeDomain(constraint->gauche->affectation->curDomain[i], 
								i, constraint->gauche->affectation, curAff);
					}
				}
				
				if (constraint->gauche->affectation->curDomainSize > 0) 
					return 1;
				else 
					return 0;
			}
			break;
			
			
			
		case SUP:
			if (constraint->gauche->affectation == curAff) {
				for (i = 0; i < lineSize; i++){
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE &&
							!(curAff->curValue > constraint->droite->affectation->curDomain[i])) {
						
						changeDomain(constraint->droite->affectation->curDomain[i], 
								i, constraint->droite->affectation, curAff);
					}
				}
				
				
				if (constraint->droite->affectation->curDomainSize > 0) 
					return 1;
				else 
					return 0;
			}
			else {
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE && 
							!(constraint->gauche->affectation->curDomain[i] > curAff->curValue)) {
						
						changeDomain(constraint->gauche->affectation->curDomain[i], 
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
				for (i = 0; i < lineSize; i++){
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE && 
							!(curAff->curValue < constraint->droite->affectation->curDomain[i])) {
						
						changeDomain(constraint->droite->affectation->curDomain[i], 
								i, constraint->droite->affectation, curAff);
					}
				}
				
				if (constraint->droite->affectation->curDomainSize > 0) 
					return 1;
				else 
					return 0;
			}
			else {
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE && 
							!(constraint->gauche->affectation->curDomain[i] < curAff->curValue)) {
						
						changeDomain(constraint->gauche->affectation->curDomain[i], 
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

/**
 * Cancel every modifications made by an affectation.
 * @param curAff The affectation to cancel.
 */
void rewriteDomain(AFFECTATION * curAff) {
	
	if (DEBUG_FC)
		printf("backing %zu\n", curAff->var->ind);
	int i;
	// For all modifications
	for (i = 0; i < curAff->compt; i++) {
		OLD_DOM *old = &changedDomain[curAff->var->ind][i];
		old->origin->curDomain[old->indice] = old->oldDomainValue;
		old->origin->curDomainSize++;
		if (DEBUG_FC)
			printf("adding %d to var %zu\n", old->oldDomainValue, old->origin->var->ind);
	}
	curAff->compt = 0;
}

/**
 * Choose the next consistant value.
 * @param curAff The current affectation.
 * @param lineSize The size of one line in the grid.
 * @param contraintes All the constraints
 * @return 1 if a consistant value has been found, 0 otherwise.
 */
int chooseNextValueFC(AFFECTATION *curAff, size_t lineSize, CONTRAINTE *contraintes) {

	int consistant = 0;
	size_t j, k;
	CONTRAINTE *curConst;
	// Test domain values while it is not consistant
	for (j = 0; j < lineSize && !consistant; ++j) {

		consistant = 0;
		if (curAff->curDomain[j] != NO_DOMAINE) {

			curAff->curValue = curAff->curDomain[j];
			curAff->curDomain[j] = NO_DOMAINE;
			curAff->curDomainSize--;

			// Check constraint
			consistant = 1;
			// Test all this var constraints with this value
			for (k = 0; k < curAff->var->indLastConst && consistant; ++k) {
				curConst = &contraintes[curAff->var->conts[k]];

				nbConstraintTestedFC++;
				
				if (!checkConstraintFC(curConst, curAff, lineSize)) {
					consistant = 0;
					// If the affectation wasn't concistant, we rollback what we did
					rewriteDomain(curAff);
					if (DEBUG_FC) {
						printf("failed for %d: \n", curAff->curValue);
						displayAffectationFC(curAff, lineSize);
					}
				}
			}
		}
	}
	return consistant;
}

/**
 * Apply forward checking algorithim to solve futoshiki grid
 * @param grid  The grid to solve
 * @param lineSize The size of one line in the grid
 * @param contraintes The constraints of the gride var
 * @param nbContraintes The number of constraint of the grid var
 * @return 1 if solution found, 0 otherwise
 */
int forwardChecking(CASE *grid, size_t lineSize,
		CONTRAINTE *contraintes, size_t nbContraintes) {

	if (nbContraintes == 0 || contraintes == NULL || grid == NULL ||
			lineSize == 0) {
		return ERROR;
	}

	size_t i;
	affectations = malloc(lineSize * lineSize * sizeof (AFFECTATION));

	for (i = 0; i < lineSize * lineSize; ++i) {
		AFFECTATION * curAff = &affectations[i];
		CASE *curCase = &grid[i];
		curAff->var = curCase;
		curAff->curDomain = malloc(lineSize * sizeof (int));
		memmove(curAff->curDomain, curCase->domaine, lineSize * sizeof (int));
		
		// If there is a domain constraint
		if(curAff->curDomain[0] == NO_DOMAINE || (lineSize > 1 && curAff->curDomain[1] == NO_DOMAINE)) {
			curAff->curDomainSize = 1;
		}
		else {
			curAff->curDomainSize = lineSize;
		}
		
		curAff->previousDomain = malloc(lineSize * sizeof(int));
		curAff->curValue = NO_DOMAINE;
		curAff->compt = 0;
		curCase->affectation = curAff;
	}
	
	changedDomain = malloc(lineSize * lineSize * sizeof (OLD_DOM*));
	for (i = 0; i < lineSize * lineSize; ++i) {
		AFFECTATION * curAff = &affectations[i];
		changedDomain[i] = malloc(curAff->var->indLastConst * lineSize * sizeof(OLD_DOM));
	}

	// Main loop
	int success = 1;
	size_t currentVarInd, k;
	int consistant = 1;
	for (currentVarInd = 0; currentVarInd < lineSize * lineSize; ++currentVarInd) {
		AFFECTATION *curAff = &affectations[currentVarInd];
		
		if (DEBUG_FC)
			printf("var : %zu\n", currentVarInd);

		// Affectation
		consistant = 0;
		
		if (DEBUG_FC)
			displayAffectationFC(curAff, lineSize);
		
		consistant = chooseNextValueFC(curAff, lineSize, contraintes);
		if (consistant) {
			// If we go deeper, we need to save the next var domain
			// So when it goes up, it can undo the current affectation tested
			if (currentVarInd + 1 < lineSize* lineSize) {
				curAff = &affectations[currentVarInd + 1];

				// We save the current domain
				memmove(curAff->previousDomain, curAff->curDomain, 
					lineSize * sizeof (int));

				curAff = &affectations[currentVarInd];
			}

			if (DEBUG_FC)
				printf("consistant pour %zu avec la valeur %d\n",
					curAff->var->ind, curAff->curValue);
			
			nbNodesFC++;
		}				
		// if we didn't found a concistant affectation
		if (!consistant) {
			// If we backtracked to root and still not consistant then we failed
			if (currentVarInd == 0) {
				printf("failure\n");
				success = 0;
				break;
			}
			// otherwise we go up
			else {
				if (DEBUG_FC) {
					printf("failure for %zu with value %d\n", currentVarInd, curAff->curValue);
				}
				
				curAff->curValue = NO_DOMAINE;
				
				// When we go up, we need to undo the values that we just have tested without changing what
				// other affectation have made
				memmove(curAff->curDomain, curAff->previousDomain, lineSize * sizeof (int));
				// Recalculate the valid domain size
				curAff->curDomainSize = 0;
				for(k = 0; k < lineSize; ++k) {
					if(curAff->curDomain[k] != NO_DOMAINE) {
						curAff->curDomainSize++;
					}
				}
				
				// We also need to rewrite things that have been to change by previous var before 
				// taking another value
				curAff = &affectations[currentVarInd - 1];
				rewriteDomain(curAff);
				
				// Going up
				currentVarInd -= 2;
				if (DEBUG_FC) {
					printf("now backing to %zu\n", currentVarInd + 1);
				}
			}
		}
	}
	
	// If we found a solution
	if (success) {
		printf("One possible solution is :\n");
		// Display the solution found
		for (i = 0; i < lineSize * lineSize; ++i) {

			AFFECTATION * curAff = &affectations[i];
			printf("%d", curAff->curValue);
			if ((i + 1)  % lineSize == 0)
				printf("\n");
			else
				printf(" ");
		}
	}
	else {
		printf("Failure : no solution\n");
	}
	
	printf("Node in the corresponding tree : %d\n", nbNodesFC);
	printf("Number of constraint test : %d\n", nbConstraintTestedFC);
		
	freeFC(grid, lineSize);
	return success;
}
