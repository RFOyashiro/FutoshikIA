#include "define.h"
#include "backTrack.h"
#include "forwardChecking.h"
#include "timer.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

clock_t begin, end;

// Total number of constraint
size_t nbContraintes = 0;
// The array of constraint
CONTRAINTE * contraintes = NULL;

// The grid itself
CASE * grid = NULL;

// The size of a line of the grid
size_t lineSize = 0;

/**
 * Open a grid file. No real sanity check here.
 * @param fileName The name of the file. Can be relative.
 * @return The FILE* corresponding of the opened file. NULL if failed to open.
 */
FILE * openFile(char * fileName) {
	FILE * grille = fopen(fileName, "r");

	return grille;
}

/**
 * Close an opened grid file. No sanity check here.
 * @param file The opened file to close
 */
void closeFile(FILE * file) {
	fclose(file);
}

/**
 * Get the char corresponding to the binary operator.
 * @param op The binary operator
 * @return  ! = DIF, > = SUP, < = INF, E = error
 */
char getOpChar(enum operateur op) {
	switch (op) {
		case DIF:
			return '!';
		case SUP:
			return '>';
		case INF:
			return '<';
		default:
			return 'E';
	}
}

/**
 * Display a constraint for debug purpose.
 * @param numConst The number of the constraint. Not necessarely, its just for display convinience
 * @param constr The constraint to display
 */
void displayConstraint(size_t numConst, CONTRAINTE * constr) {
	printf("CONSTRAINT num %zu : ", numConst);
	char op = getOpChar(constr->op);
	printf("    %zu", constr->gauche->ind);
	printf(" %c ", op);
	printf("%zu\n", constr->droite->ind);
}

/**
 * Display a variable (slot) for debug purpose.
 * @param caseToDisplay The variable to display.
 */
void displayCase(CASE *caseToDisplay) {
	printf("CASE num %zu\n", caseToDisplay->ind);
	printf("    domaine = {");
	int i;
	for (i = 0; i < lineSize; ++i) {
		printf("%d", caseToDisplay->domaine[i]);
		if (i < lineSize - 1)
			printf(", ");
	}
	printf("}\n");
}

/**
 * Display all the variables of the grid for debug purpose.
 */
void displayGrid() {
	size_t i;
	for (i = 0; i < lineSize * lineSize; ++i) {
		CASE * currentCase = &grid[i];
		displayCase(currentCase);
		printf("\n");
	}
}

/**
 * Initialize all the variables. Malloc them, malloc their domain and malloc their conts. lineSize must be initialize.
 * @param grid The grid to take variable of.
 */
void initAllCases(CASE * grid) {
	size_t i;
	for (i = 0; i < lineSize * lineSize; ++i) {
		CASE currentCase;

		currentCase.ind = i;

		// Initialisation du domaine
		currentCase.domaine = malloc(lineSize * sizeof (int));
		int m;
		for (m = 0; m < lineSize; ++m) {
			currentCase.domaine[m] = m + 1;
		}

		currentCase.conts = malloc((2 * (lineSize - 1)) * sizeof (size_t));
		currentCase.indLastConst = 0;
		currentCase.affectation = NULL;

		grid[i] = currentCase;
	}
}

/**
 * Init all constraints, assigning left and right operands (and make them point to this constraint). By default all constraint
 * are DIF constraint. lineSize must be initialize.
 * @param grid The gride to take variables of. 
 */
void initBasicConstraints(CASE *grid) {

	size_t numCurrentCase = 0;
	size_t indEndLine;
	size_t lastConstInd = 0;
	int lineNum = 0;

	for (numCurrentCase = 0; numCurrentCase < lineSize * lineSize; ++numCurrentCase) {
		CASE * currentCase = &grid[numCurrentCase];

		if (DEBUG_INIT_CONST) {
			displayCase(currentCase);
			printf("\n");
		}

		indEndLine = numCurrentCase;

		// Recherche de l'indice de la case de debut de la prochaine ligne si != debut de ligne
		if (indEndLine % lineSize == 0) {
			indEndLine += lineSize;
		}
		else {
			while (!(indEndLine % lineSize == 0)) {

				indEndLine++;
			}
		}

		if (DEBUG_INIT_CONST) {
			printf("indEndLine : %zu\n", indEndLine);
		}

		// Ajout de toutes les contraintes de la ligne courante a 
		// partir de la case courrante car les cases precedentes ont deja misent
		// les contraintes sur celle-ci (pas de redondance des contraintes)
		size_t indCaseDroite = numCurrentCase + 1;
		for (; indCaseDroite < indEndLine; ++indCaseDroite) {

			CASE * caseDroite = &grid[indCaseDroite];

			CONTRAINTE cont;
			cont.gauche = currentCase;
			cont.droite = caseDroite;
			cont.op = DIF;

			contraintes[lastConstInd] = cont;
			currentCase->conts[currentCase->indLastConst++] = lastConstInd;
			caseDroite->conts[caseDroite->indLastConst++] = lastConstInd;

			if (DEBUG_INIT_CONST) {
				printf("contrainte de ligne : ");
				displayConstraint(lastConstInd, &cont);
				printf("droite : \n");
				if (numCurrentCase < 4) {
					size_t o;
					for (o = 0; o < caseDroite->indLastConst; ++o) {
						displayConstraint(caseDroite->conts[o],
								&(contraintes[caseDroite->conts[o]]));
					}
				}
			}
			lastConstInd++;
		}

		if (numCurrentCase % lineSize == 0) {
			lineNum++;
		}

		// Ajout de toutes les contraintes de la colonne courante
		int i = lineNum;
		size_t lineCpt = 1;
		for (; i < lineSize; ++i) {

			if (DEBUG_INIT_CONST) {
				printf("lineNum = %d, i = %d\n", lineNum, i);
			}

			CASE * caseDroite = &grid[numCurrentCase + (lineSize * lineCpt)];

			CONTRAINTE cont;
			cont.gauche = currentCase;
			cont.droite = caseDroite;
			cont.op = DIF;

			contraintes[lastConstInd] = cont;
			currentCase->conts[currentCase->indLastConst++] = lastConstInd;
			caseDroite->conts[caseDroite->indLastConst++] = lastConstInd;

			if (DEBUG_INIT_CONST) {
				printf("contrainte de colonne : ");
				displayConstraint(lastConstInd, &cont);
			}
			lastConstInd++;
			lineCpt++;
		}

	}

	if (DEBUG_INIT_CONST) {
		printf("nbConstr : %zu, lastConst : %zu\n", nbContraintes, lastConstInd);
	}
}

/**
 * Function called if an error has been found while initializing the grid. Espacially if the file is malformed.
 * @param line The current line
 * @param msg The error message
 * @return -1
 */
int failInitGrid(char *line, char *msg) {
	fprintf(stderr, "%s\n", msg);
	if (line)
		free(line);
	return -1;
}

/**
 * Initialize the grid with all variables and constraints.
 * @param gridFile The grid file
 * @return -1 if failed, 1 if succeded
 */
int initGrid(FILE *gridFile) {
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	read = getline(&line, &len, gridFile);
	if (read <= 0) {
		return failInitGrid(line, "Grid file empty.");
	}

	if (sscanf(line, "%zu", &lineSize) != 1) {
		return failInitGrid(line, "Grid size must be an unsigned number.");
	}

	if (DEBUG_INIT_CONST)
		printf("Grid size : %zu\n", lineSize);

	grid = malloc((lineSize * lineSize) * sizeof (CASE));

	nbContraintes = ((lineSize - 1) * lineSize * lineSize);
	contraintes = malloc(nbContraintes * sizeof (CONTRAINTE));

	// initialisation des cases
	initAllCases(grid);

	if (DEBUG_INIT_CONST) {
		printf("Grid initialized\n");
		displayGrid();
		printf("\n");
	}
	// initialisation des contraintes
	initBasicConstraints(grid);

	if (line)
		free(line);

	return 1;
}

/**
 * Pars a line which should contains domain constraint.
 * @param line The line to parse
 * @param lineNumber The number of the line
 * @return 1 if succeed, -1 if failed (espacially if file is malformed)
 */
int parseNumberLine(char* line, size_t lineNumber) {
	size_t charNumber;
	for (charNumber = 0; charNumber < lineSize * 2; ++charNumber) {
		// Si on doit changer une contrainte
		if (line[charNumber] == '<' || line[charNumber] == '>') {
			if (charNumber == 0 || charNumber == lineSize * 2) {
				return failInitGrid(line,
						"Constraint without both values detected");
			}
			CASE *left = &grid[charNumber / 2 + (lineNumber / 2) * lineSize];

			// Recherche de la premiere contrainte ou left est a gauche
			// Sachant que ce sera au minium la premiere apres le nombre 
			// de case a gauche
			size_t i;
			CONTRAINTE *constraint;
			for (i = charNumber / 2;; ++i) {
				constraint = &contraintes[left->conts[i]];
				if (constraint->gauche == left) {
					break;
				}
			}

			if (line[charNumber] == '<')
				constraint->op = INF;
			else
				constraint->op = SUP;

			if (DEBUG_FILE_READING) {
				displayConstraint(left->conts[i], constraint);
			}
		}
		// C'est un nombre
		else if (line[charNumber] != ' ') {
			int number;
			if (sscanf(&line[charNumber], "%d", &number) != 1) {
				printf("Unknown char detected on a number at line %zu at char : %zu\n", lineNumber, charNumber);
				return failInitGrid(line,
						"Unknown char detected on a number line");
			}

			if (number != 0) {
				CASE *currentCase =
						&grid[charNumber / 2 + (lineNumber / 2) * lineSize];
				size_t i;
				// On enleve toutes les valeurs du domaine
				for (i = 0; i < lineSize; ++i) {
					if (currentCase->domaine[i] == number)
						continue;
					currentCase->domaine[i] = NO_DOMAINE;
				}
				if (DEBUG_FILE_READING)
					displayCase(currentCase);
			}
		}
	}
	return 1;
}

/**
 * Pars a line which should contain only SUP or INF constraint.
 * @param line The line to parse
 * @param lineNumber The number of line.
 * @return 1 if succeeded, -1 if failed (especially if file malformed)
 */
int parseConstraintOnlyLine(char *line, size_t lineNumber) {
	size_t charNumber;
	for (charNumber = 0; charNumber < lineSize; ++charNumber) {
		if (line[charNumber] != '.') {
			CASE *left = &grid[charNumber + (lineNumber / 2) * lineSize];
			CASE *right = &grid[charNumber + ((lineNumber + 1) / 2) * lineSize];
			// Recherche la premiere contrainte de colonne ou left 
			// est a gauche et right est a droite
			// On sait que ce sera au minimum la premiere apres le 
			// nombre de case a droite
			CONTRAINTE *constraint;
			size_t i;
			for (i = (lineSize - 1) - charNumber;; ++i) {
				constraint = &contraintes[left->conts[i]];
				if (constraint->gauche == left && constraint->droite == right) {
					break;
				}
			}
			if (line[charNumber] == '^') {
				constraint->op = INF;
			}
			else if (line[charNumber] == 'v') {
				constraint->op = SUP;
			}
			else {
				return failInitGrid(line,
						"Unknown char detected on constraint only line");
			}

			if (DEBUG_FILE_READING)
				displayConstraint(left->conts[ (lineSize - 1) - charNumber],
					constraint);
		}
	}
	return 1;
}

/**
 * Read a grid file and parse it to modifie variables and constraints. lineSize must be initialized.
 * @param gridFile The file of the grid.
 * @return 1 if succeeded, -1 if failed (especially if file malformed)
 */
int readAndParseFile(FILE *gridFile) {
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	int isLineWithNumber = 1;
	size_t lineNumber;
	for (lineNumber = 0; lineNumber < lineSize * 2 - 1; ++lineNumber) {
		read = getline(&line, &len, gridFile);
		if (read <= 0) {
			return failInitGrid(line, "Grid file malformed");
		}

		if (DEBUG_FILE_READING)
			printf("line read :%s", line);

		if (len < lineSize * 2) {
			return failInitGrid(line, "Line malformed");
		}

		int res;
		if (isLineWithNumber) {
			res = parseNumberLine(line, lineNumber);
			if (res < 0) {
				return res;
			}
		}
		else {
			res = parseConstraintOnlyLine(line, lineNumber);
			if (res < 0) {
				return res;
			}
		}

		isLineWithNumber = 1 - isLineWithNumber;
	}
	if (line)
		free(line);
	return 1;
}

/**
 * Free a variable
 * @param caseToFree The variable to free.
 */
void freeCase(CASE * caseToFree) {

	free(caseToFree->domaine);
	free(caseToFree->conts);
}

/**
 * Free the grid and the constraints
 */
void freeGridAndConstraints() {
	if (DEBUG_INIT_CONST)
		printf("Liberation memoire\n");

	size_t i;
	for (i = 0; i < lineSize * lineSize; ++i) {
		freeCase(&grid[i]);
	}
	free(grid);
	free(contraintes);
}

/**
 * The main
 * @param argc Number of args
 * @param argv The args (argv[1] should be the grid file name)
 * @return 0 if success, -1 if failure
 */
int main(int argc, char * argv[]) {
	if (argc - 1 < 1) {
		fprintf(stderr, "Error on argument. Usage :\nfutoshiki <GridfileName>\n");
		exit(EXIT_FAILURE);
	}

	FILE *gridFile = openFile(argv[1]);
	if (gridFile == NULL) {
		fprintf(stderr, "Impossible d'ouvrir le fichier\n");
		exit(EXIT_FAILURE);
	}

	int res = initGrid(gridFile);
	if (res < 0) {
		fprintf(stderr, "Erreur de creation de la grille\n");
		exit(EXIT_FAILURE);
	}

	res = readAndParseFile(gridFile);
	if (res < 0) {
		fprintf(stderr, "Erreur de lecture du fichier\n");
		goto end;
	}
	
	int Result[3];

	begin = clock();
	
	printf("FC\n");
	res = forwardChecking(grid, lineSize, contraintes, nbContraintes);
	
	end = clock();
	getTimeElapsed(begin, end, Result);
	printf("time : %02d:%02d:%03d ms\n\n", Result[0], Result[1], Result[2]);
	
	if (res < 0) {
		fprintf(stderr, "Erreur de backtrack\n");
		goto end;
	}
	
	begin = clock();
	
	
	printf("backTrack\n");
	res = backTrack(grid, lineSize, contraintes, nbContraintes);	
	
	end = clock();
	getTimeElapsed(begin, end, Result);
	printf("time : %02d:%02d:%03d ms\n\n", Result[0], Result[1], Result[2]);
	
	if (res < 0) {
		fprintf(stderr, "Erreur de forwardChecking\n");
		goto end;
	}

end:
	// Liberation memoire
	freeGridAndConstraints();
	closeFile(gridFile);
	return (0);
}
