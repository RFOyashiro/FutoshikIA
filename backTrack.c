#include "backTrack.h"

/**
 * All the variables affectatons
 */
AFFECTATION *affectations;

int nbNodesBT = 1;
int nbConstraintTestedBT = 0;

/**
 * Free backtrack affectations
 */
void freeBackTrack(CASE *grid, size_t lineSize) {
	size_t i;
	for (i = 0; i < lineSize * lineSize; ++i) {
		(&grid[i])->affectation = NULL;
		AFFECTATION *curAff = &affectations[i];
		free(curAff->curDomain);
	}
	free(affectations);
}

/**
 * Display an affectation for debug purpose
 * @param affect The affectations
 * @param lineSize The size of a line
 */
void displayAffectation(AFFECTATION *affect, size_t lineSize) {

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
	printf("	current Value = %d\n\n", affect->curValue);

}

/**
 * Check if a constraint is concistant
 * @param constraint The constraint to check
 * @return 1 if concistant, 0 otherwise or if constraint is about not affected variables.
 */
int checkConstraint(CONTRAINTE *constraint) {
	// If the constraint is about a non affected var
	if (constraint->gauche->affectation->curValue == NO_DOMAINE ||
			constraint->droite->affectation->curValue == NO_DOMAINE) {
		return 1;
	}
	switch (constraint->op) {
		case DIF:
			return constraint->gauche->affectation->curValue !=
					constraint->droite->affectation->curValue;
		case SUP:
			return constraint->gauche->affectation->curValue >
					constraint->droite->affectation->curValue;
		case INF:
			return constraint->gauche->affectation->curValue <
					constraint->droite->affectation->curValue;
		default:
			return ERROR;
	}
}

/**
 * Apply backtrack technic to try to solve the futushiki grid
 * @param grid Futushiki grid
 * @param lineSize The size of one line of variables
 * @param contraintes The constraints
 * @param nbContraintes The number of constraint
 * @return 1 if succeeded, -1 if failed (or no solution)
 */
int backTrack(CASE *grid, size_t lineSize,
		CONTRAINTE *contraintes, size_t nbContraintes) {

	if (nbContraintes == 0 || contraintes == NULL || grid == NULL ||
			lineSize == 0) {
		return ERROR;
	}

	affectations = malloc(lineSize * lineSize * sizeof (AFFECTATION));

	size_t i;
	for (i = 0; i < lineSize * lineSize; ++i) {
		AFFECTATION * curAff = &affectations[i];
		CASE *curCase = &grid[i];
		curAff->var = curCase;
		curAff->curDomain = malloc(lineSize * sizeof (int));
		memmove(curAff->curDomain, curCase->domaine, lineSize * sizeof (int));
		curAff->curValue = NO_DOMAINE;

		curCase->affectation = curAff;
	}

	// Main loop
	int success = 1;
	size_t currentVarInd;
	int consistant = 1;
	for (currentVarInd = 0; currentVarInd < lineSize * lineSize; ++currentVarInd) {
		AFFECTATION *curAff = &affectations[currentVarInd];

		int j;

		// Affectation
		consistant = 0;
		for (j = 0; j < lineSize && !consistant; ++j) {

			if (curAff->curDomain[j] != NO_DOMAINE) {

				curAff->curValue = curAff->curDomain[j];
				curAff->curDomain[j] = NO_DOMAINE;

				// Check constraint
				size_t k;
				consistant = 1;
				for (k = 0; k < curAff->var->indLastConst && consistant; ++k) {
					CONTRAINTE *curConst = &contraintes[curAff->var->conts[k]];
					
					nbConstraintTestedBT++;
					
					if (!checkConstraint(curConst)) {
						consistant = 0;
					}
				}
				
				if (consistant) {
					
					nbNodesBT++;
					if (DEBUG_BACKTRACK) {
						printf("consistant pour %zu avec la valeur %d\n", currentVarInd, curAff->curValue);
					}
				}
			}

		}
		// if domain empty the we didn't find a valid affectation so backtrack
		if (!consistant) {
			// If we backtracked to root and still not consistant then we failed
			if (currentVarInd == 0) {
				if(DEBUG_BACKTRACK)
					printf("failure\n");
				success = 0;
				break;
			}
			else {
				if (DEBUG_BACKTRACK) {
					printf("failure for %zu with value %d\n", currentVarInd, curAff->curValue);
				}
				curAff->curValue = NO_DOMAINE;
				memmove(curAff->curDomain, curAff->var->domaine,
						lineSize * sizeof (int));
				currentVarInd -= 2;
			}
		}

	}

	if (success) {
		printf("One possible solution is :\n");
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
	
	printf("Node in the corresponding tree : %d\n", nbNodesBT);
	printf("Number of constraint test : %d\n", nbConstraintTestedBT);

	freeBackTrack(grid, lineSize);
	return success;
}

