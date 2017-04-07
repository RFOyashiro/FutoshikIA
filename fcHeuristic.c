#include "fcHeuristic.h"

/**
 * All the variables affectatons
 */
AFFECTATION *affectationsFCH;

OLD_DOM **changedDomainFCH;

size_t affectedVarInd = 0;
AFFECTATION **affectedVar;

size_t oneValueVarInd = 0;
AFFECTATION **oneValueVar;

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
	printf("	current Value = %d\n", affect->curValue);
	printf("	compt = %d\n\n", affect->compt);

}

void freeFCH(CASE *grid, size_t lineSize) {
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
	/*free(affectedVar);
	free(oneValueVar);*/
}

void changeDomainFCH(int oldValue, size_t indice, AFFECTATION *origin, AFFECTATION *modifier) {
	OLD_DOM old;
	old.oldDomainValue = oldValue;
	old.indice = indice;
	old.origin = origin;
	changedDomainFCH[modifier->var->ind][modifier->compt++] = old;
	if (DEBUG_FCH)
		printf("removed %d from var %zu\n", oldValue, origin->var->ind);
	origin->curDomain[indice] = NO_DOMAINE;
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

				for (i = 0; i < lineSize; i++) {
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
				return 0;
			}
			else {
				for (i = 0; i < lineSize; i++) {
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE &&
							constraint->gauche->affectation->curDomain[i] == curAff->curValue) {

						changeDomainFCH(constraint->gauche->affectation->curDomain[i],
								i, constraint->gauche->affectation, curAff);
					}
				}

				for (i = 0; i < lineSize; i++) {
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
				return 0;
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


				for (i = 0; i < lineSize; i++) {
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
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

				for (i = 0; i < lineSize; i++) {
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
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

				for (i = 0; i < lineSize; i++) {
					if (constraint->droite->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
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

				for (i = 0; i < lineSize; i++) {
					if (constraint->gauche->affectation->curDomain[i] != NO_DOMAINE) return 1;
				}
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
		if (DEBUG_FCH)
			printf("adding %d to var %zu\n", old->oldDomainValue, old->origin->var->ind);
	}
	curAff->compt = 0;
}

AFFECTATION *chooseNextVar(CASE *grid, size_t lineSize,
		CONTRAINTE *contraintes, size_t nbContraintes) {

	AFFECTATION *curAff = NULL;
	size_t i;

	if (oneValueVarInd > 0) {
		// Test if there is a non affected var with value constraint
		for (i = 0; i < oneValueVarInd - 1; ++i) {
			if (oneValueVar[i]->curValue == NO_DOMAINE) {
				curAff = oneValueVar[i];
				return curAff;
			}
		}
	}

	for (i = 0; i < lineSize * lineSize; ++i) {
		if ((&affectationsFCH[i])->curValue == NO_DOMAINE) {
			curAff = &affectationsFCH[i];
			return curAff;
		}
	}
	return curAff;
}

int fcHeuritic(CASE *grid, size_t lineSize,
		CONTRAINTE *contraintes, size_t nbContraintes) {

	if (nbContraintes == 0 || contraintes == NULL || grid == NULL ||
			lineSize == 0) {
		return ERROR;
	}

	size_t i;
	affectationsFCH = malloc(lineSize * lineSize * sizeof (AFFECTATION));
	changedDomainFCH = malloc(lineSize * lineSize * sizeof (OLD_DOM*));
	affectedVar = malloc(lineSize * lineSize * sizeof (AFFECTATION*));
	oneValueVar = malloc(lineSize * lineSize * sizeof (AFFECTATION*));

	for (i = 0; i < lineSize * lineSize; ++i) {
		AFFECTATION * curAff = &affectationsFCH[i];
		CASE *curCase = &grid[i];
		curAff->var = curCase;
		curAff->curDomain = malloc(lineSize * sizeof (int));
		memmove(curAff->curDomain, curCase->domaine, lineSize * sizeof (int));
		curAff->previousDomain = malloc(lineSize * sizeof (int));
		curAff->curValue = NO_DOMAINE;
		curAff->compt = 0;
		curCase->affectation = curAff;

		changedDomainFCH[i] = malloc(curAff->var->indLastConst * lineSize * sizeof (OLD_DOM));

		// If this var has a value constraint
		if (curAff->curDomain[0] == NO_DOMAINE) {
			oneValueVar[oneValueVarInd] = curAff;
			oneValueVarInd++;
		}
	}

	if (DEBUG_FCH)
		printf("Initialization completed. Beginning main loop\n");

	// Main loop
	int success = 1;
	int consistant = 1;
	AFFECTATION *curAff = chooseNextVar(grid, lineSize, contraintes, nbContraintes);
	affectedVar[affectedVarInd++] = curAff;
	while (curAff != NULL) {

		if (DEBUG_FCH) {
			printf("var : %zu\n", curAff->var->ind);
		}

		// Affectation
		int j;
		consistant = 0;

		if (DEBUG_FCH) {
			displayAffectationFCH(curAff, lineSize);
		}
		for (j = 0; j < lineSize; ++j) {

			if (curAff->curDomain[j] != NO_DOMAINE) {

				curAff->curValue = curAff->curDomain[j];
				curAff->curDomain[j] = NO_DOMAINE;

				// Check constraint
				size_t k;
				consistant = 1;

				for (k = 0; k < curAff->var->indLastConst && consistant; ++k) {
					CONTRAINTE *curConst = &contraintes[curAff->var->conts[k]];

					if (!checkConstraintFCH(curConst, curAff, lineSize)) {
						consistant = 0;
					}

				}

				// If we found a concistant affectation
				if (consistant) {

					affectedVar[affectedVarInd] = curAff;
					affectedVarInd++;

					// If we go deeper, we need to save the next var domain
					// So when it goes up, it can undo the current affectation tested
					AFFECTATION *nextAff = chooseNextVar(grid, lineSize, contraintes, nbContraintes);
					if (nextAff != NULL) {
						// We save the current domain
						memmove(nextAff->previousDomain, nextAff->curDomain,
								lineSize * sizeof (int));
					}

					if (DEBUG_FCH)
						printf("consistant pour %zu avec la valeur %d\n",
							curAff->var->ind, curAff->curValue);

					break;
				}
				else {
					// If the affectation wasn't concistant, we rollback what we did
					rewriteDomainFCH(curAff, lineSize);
					if (DEBUG_FCH) {
						printf("failed for %d: \n", curAff->curValue);
						displayAffectationFCH(curAff, lineSize);
					}
				}
			}

		}

		//freeFCH(previousDomain);

		// if we didn't found a concistant affectation
		if (!consistant) {
			// If we backtracked to root and still not consistant then we failed
			AFFECTATION *nextAff = chooseNextVar(grid, lineSize, contraintes, nbContraintes);
			if (nextAff->var->ind == 0) {
				printf("failure\n");
				success = 0;
				break;
			}
				// otherwise we go up
			else if (affectedVarInd > 0) {
				if (DEBUG_FCH) {
					printf("failure for %zu with value %d\n", curAff->var->ind, curAff->curValue);
				}

				curAff->curValue = NO_DOMAINE;

				// When we go up, we need to undo the values that we just have tested without changing what
				// other affectation have made
				memmove(curAff->curDomain, curAff->previousDomain, lineSize * sizeof (int));

				// We also need to rewrite things that have been to change by previous var before 
				// taking another value
				AFFECTATION *previous = affectedVar[affectedVarInd - 1];
				rewriteDomainFCH(previous, lineSize);

				// Going up
				previous->curValue = NO_DOMAINE;
				affectedVarInd--;

				if (DEBUG_FCH) {
					printf("now backing to %zu\n", previous->var->ind);
				}
			}
		}
		curAff = chooseNextVar(grid, lineSize, contraintes, nbContraintes);
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

	if (DEBUG_FCH) {
		printf("Freeing\n");
	}
	freeFCH(grid, lineSize);
	return success;
}
