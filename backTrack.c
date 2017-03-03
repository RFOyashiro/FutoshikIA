#include "backTrack.h"

AFFECTATION *affectations;

void freeBackTrack(size_t lineSize) {
	size_t i;
	for(i = 0; i < lineSize * lineSize; ++i) {
		AFFECTATION * curAff = &affectations[i];
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
		printf("%d", caseToDisplay->domaine[j]);
		if (j < lineSize - 1)
			printf(", ");
	}
	printf("}\n");
	printf("	current Value = %d\n\n", affect->curValue);
	
}

int checkConstraint(CONTRAINTE *constraint, int leftValue, int rightValue) {
	switch (constraint->op) {
		case DIF:
			return leftValue != rightValue;
		case SUP:
			return leftValue > rightValue;
		case INF:
			return leftValue < rightValue;
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
	
	affectations = malloc(lineSize * lineSize * sizeof(AFFECTATION));
	
	size_t i;
	for(i = 0; i < lineSize * lineSize; ++i) {
		AFFECTATION * curAff = &affectations[i];
		CASE *curCase = &grid[i];
		curAff->var = curCase;
		curAff->curDomain = malloc(lineSize * sizeof(int));
		memmove(curAff->curDomain, curCase->domaine, sizeof(int));
		curAff->curValue = NO_DOMAINE;
		
		curCase->affectation = curAff;
	}
	
	if (DEBUG_BACKTRACK) {
		for (i = 0; i < lineSize * lineSize; ++i) {
	
			AFFECTATION * curAff = &affectations[i];
			displayAffectation(curAff, lineSize);
		}
	}
	
	// Main loop
	int success = 0;
	size_t currentVarInd;
	for (currentVarInd = 0; currentVarInd < lineSize * lineSize; ++currentVarInd) {
		AFFECTATION *curAff = &affectations[i];
		int j;
		// Affectation
		for (j = 0; i < lineSize; ++j) {
			if (curAff->curDomain == NO_DOMAINE) {
				continue;
			}
			curAff->curValue = curAff->curDomain[j];
			&curAff->curDomain[j] = NO_DOMAINE;
			
			// Check constraint
			int consistant = 0;
			size_t k;
			for(k = 0; k < curAff->var->indLastConst; ++k) {
				CONTRAINTE *curConst = &contraintes[k];
				// If the constraint is about a non affected var
				if (curConst->left->affectation == NULL || 
					curConst->right->affectation == NULL) {
					continue;
				}
				//if(!checkConstraint(curConst, curAff->curValue, 
			}
		}
		// if domain empty
		if (curAff->curValue == NO_DOMAINE) {
			// TODO : back
		}
	}
	
	freeBackTrack(lineSize);
	return 1;
}

