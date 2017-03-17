#include "forwardChecking.h"

/**
 * All the variables affectatons
 */
AFFECTATION *affectations;



OLD_DOM **changedDomain;

void freeFC(CASE *grid, size_t lineSize) {
	size_t i;
	
	for (i = 0; i < lineSize * lineSize; ++i) {
		free(changedDomain[i]);
	}
	free(changedDomain);
	
	for (i = 0; i < lineSize * lineSize; ++i) {
		(&grid[i])->affectation = NULL;
		AFFECTATION *curAff = &affectations[i];
		free(curAff->curDomain);
	}
	free(affectations);
}

int checkConstraintFC(CONTRAINTE *constraint, AFFECTATION * curAff, size_t lineSize) {
	// If the constraint is about a non affected var
	int i;
	
	switch (constraint->op) {
	
	
	
		case DIF:
			if (constraint->gauche->affectation == curAff) {
				for (i = 0; i < lineSize; i++){
					if (constraint->droite->affectation->curDomain[i] == curAff->curValue) {
						OLD_DOM old;
						old.oldDomainValue = constraint->droite->affectation->curDomain[i];
						old.indice = i;
						old.origin = constraint->droite->affectation;
						changedDomain[curAff->var->ind][curAff->compt++] = old;
						constraint->droite->affectation->curDomain[i] = NO_DOMAINE;
						
					}
				}
				
				for (i = 0; i < lineSize; i++){
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
				return 0;
			}
			else {
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] == curAff->curValue) {
						OLD_DOM old;
						old.oldDomainValue = constraint->gauche->affectation->curDomain[i];
						old.indice = i;
						old.origin = constraint->gauche->affectation;
						changedDomain[curAff->var->ind][curAff->compt++] = old;
						constraint->gauche->affectation->curDomain[i] = NO_DOMAINE;
					}
				}
				
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
				return 0;
			}
			break;
			
			
			
		case SUP:
			if (constraint->gauche->affectation == curAff) {
				for (i = 0; i < lineSize; i++){
					if (constraint->droite->affectation->curDomain[i] < curAff->curValue) {
						OLD_DOM old;
						old.oldDomainValue = constraint->droite->affectation->curDomain[i];
						old.indice = i;
						old.origin = constraint->droite->affectation;
						changedDomain[curAff->var->ind][curAff->compt++] = old;
						constraint->droite->affectation->curDomain[i] = NO_DOMAINE;
					}
				}
				
				for (i = 0; i < lineSize; i++){
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
				return 0;
			}
			else {
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] > curAff->curValue) {
						OLD_DOM old;
						old.oldDomainValue = constraint->gauche->affectation->curDomain[i];
						old.indice = i;
						old.origin = constraint->gauche->affectation;
						changedDomain[curAff->var->ind][curAff->compt++] = old;
						constraint->gauche->affectation->curDomain[i] = NO_DOMAINE;
					}
				}
				
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
				return 0;
			}
			break;
			
			
			
			
		case INF:
			if (constraint->gauche->affectation == curAff) {
				for (i = 0; i < lineSize; i++){
					if (constraint->droite->affectation->curDomain[i] > curAff->curValue) {
						OLD_DOM old;
						old.oldDomainValue = constraint->droite->affectation->curDomain[i];
						old.indice = i;
						old.origin = constraint->droite->affectation;
						changedDomain[curAff->var->ind][curAff->compt++] = old;
						constraint->droite->affectation->curDomain[i] = NO_DOMAINE;
					}
				}
				
				for (i = 0; i < lineSize; i++){
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
				return 0;
			}
			else {
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] < curAff->curValue) {
						OLD_DOM old;
						old.oldDomainValue = constraint->gauche->affectation->curDomain[i];
						old.indice = i;
						old.origin = constraint->gauche->affectation;
						changedDomain[curAff->var->ind][curAff->compt++] = old;
						constraint->gauche->affectation->curDomain[i] = NO_DOMAINE;
					}
				}
				
				for (i = 0; i < lineSize; i++){
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
				return 0;
			}
			break;
		default:
			return ERROR;
	}
}

void rewriteDomain(AFFECTATION * curAff, size_t lineSize) {
	
	int i;
	for (i = 0; i < curAff->compt; i++) {
		OLD_DOM *old = &changedDomain[curAff->var->ind][i];
		old->origin->curDomain[old->indice] = old->oldDomainValue;		
	}
	curAff->compt = 0;
}

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
	printf("	current Value = %d\n\n", affect->curValue);

}

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
					
					if (!checkConstraintFC(curConst, curAff, lineSize)) {
						consistant = 0;
					}

				}

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
			// if go up
			else {
				if (DEBUG_FC) {
					printf("failure for %zu with value %d\n", currentVarInd, curAff->curValue);
				}
				
				curAff->curValue = NO_DOMAINE;
				memmove(curAff->curDomain, curAff->var->domaine, lineSize * sizeof (int));
				rewriteDomain(curAff, lineSize);
				currentVarInd -= 2;
			}
		}

	}

	if (DEBUG_FC) {
		if (success) {
			printf("Solution is :\n");
			for (i = 0; i < lineSize * lineSize; ++i) {
	
				AFFECTATION * curAff = &affectations[i];
				displayAffectationFC(curAff, lineSize);
			}
		}
		else {
			printf("Failure : no solution\n");
		}
	}

	freeFC(grid, lineSize);
	return success;
}
