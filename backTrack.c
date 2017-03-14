#include "backTrack.h"

AFFECTATION *affectations;

void freeBackTrack(CASE *grid, size_t lineSize) {
	size_t i;
	for (i = 0; i < lineSize * lineSize; ++i) {
		(&grid[i])->affectation = NULL;
		AFFECTATION *curAff = &affectations[i];
		free(curAff->curDomain);
	}
	free(affectations);
}

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

	/*if (DEBUG_BACKTRACK) {
		for (i = 0; i < lineSize * lineSize; ++i) {
	
			AFFECTATION * curAff = &affectations[i];
			displayAffectation(curAff, lineSize);
		}
	}*/

	// Main loop
	int success = 1;
	size_t currentVarInd;
	// TODO : ARRET TROP TOT
	int consistant = 1;
	for (currentVarInd = 0; currentVarInd < lineSize * lineSize; ++currentVarInd) {
		AFFECTATION *curAff = &affectations[currentVarInd];

		//printf("\n*********************************************\n");

		int j;

		//displayAffectation(curAff, lineSize);
		// Affectation
		consistant = 0;
		for (j = 0; j < lineSize && !consistant; ++j) {

			//printf("testing %d ", curAff->curDomain[j]);
			if (curAff->curDomain[j] != NO_DOMAINE) {

				curAff->curValue = curAff->curDomain[j];
				curAff->curDomain[j] = NO_DOMAINE;
				//printf("Value found : %d\n", curAff->curValue);

				// Check constraint
				size_t k;
				consistant = 1;
				for (k = 0; k < curAff->var->indLastConst && consistant; ++k) {
					CONTRAINTE *curConst = &contraintes[curAff->var->conts[k]];
					
					if (!checkConstraint(curConst)) {
						consistant = 0;
					}

				}

				//displayAffectation(curAff, lineSize);
				if (consistant) {
					printf("consistant pour %zu avec la valeur %d\n", currentVarInd, curAff->curValue);
				}
			}

		}
		// if domain empty the we didn't find a valid affectation so backtrack
		if (!consistant) {
			// If we backtracked to root and still not consistant then we failed
			if (currentVarInd == 0) {
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
				if (currentVarInd <= 2) {
					displayAffectation(curAff, lineSize);
				}
				currentVarInd -= 2;
			}
		}

	}

	if (DEBUG_BACKTRACK) {
		if (success) {
			printf("Solution is :\n");
			for (i = 0; i < lineSize * lineSize; ++i) {
	
				AFFECTATION * curAff = &affectations[i];
				displayAffectation(curAff, lineSize);
			}
		}
		else {
			printf("Failure : no solution\n");
		}
	}

	freeBackTrack(grid, lineSize);
	return success;
}

