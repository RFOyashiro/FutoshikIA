#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

enum operateur {DIF, SUP, INF};

typedef struct CASE{
	int x;
	int y;
	int * domaine;
	//indices de toutes les contraintes de la case dans le tableau contraintes
	int * conts;
} CASE;

typedef struct CONTRAINTE{
	enum operateur op;
	CASE * droite;
	CASE * gauche;
} CONTRAINTE;

CONTRAINTE * contraintes;

int gridSize;

FILE * openFile (char * fileName) {
	FILE * grille = fopen(fileName, "r");
	
	return grille;
}

void closeFile (FILE * file) {
	fclose(file);
}

CASE * initGrid(FILE *file) {
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    read = getline(&line, &len, file);
    if (read <= 0)
    	return NULL;
    
    gridSize = atoi(line);
    
    CASE * cases[gridSize * gridSize];
    
    int isLineWithNumber = 1;
    int comptCol = 0;
    
    int nbContraintes = ((2 * gridSize - 2) * gridSize * gridSize);
    contraintes = malloc(nbContraintes * sizeof(CONTRAINTE));
    
    // initialisation des cases
    int i;
    for (i = 0; i < gridSize * gridSize; ++i) {
    	CASE currentCase;
    	currentCase.x = i % gridSize;
    	currentCase.y = comptCol;
    	comptCol++;
    	if (i % gridSize == 0)
    		comptCol = 0;
    	
    	currentCase.domaine = malloc(gridSize * sizeof(int));
    	int m;
    	for (m = 0; m < gridSize; ++m) {
    		currentCase.domaine[m] = m + 1;
    	}
    	
    	currentCase.conts = malloc((2 * gridSize - 2) * sizeof(int));
    }
    
    int numCurrentCase = 0;
    // initialisation des contraintes
    for (i = 0; i < nbContraintes; ++i) {
    	CASE * currentCase = cases[numCurrentCase];
		
		int indDebLigne = 0;
		while (!(indDebLigne < currentCase->y * gridSize) &&
			   !(indDebLigne > (currentCase->y - 1) * gridSize)) {
			   
			indDebLigne += gridSize;		   
		}
		
		for(; indDebLigne < gridSize * currentCase->y; ++indDebLigne) {
	    	
	    	CASE * caseDroite = cases[indDebLigne];
	    	
	    	if (caseDroite != currentCase) {
				CONTRAINTE cont;
				cont.gauche = currentCase;
				cont.droite = caseDroite;
				cont.op = DIF;
	    	}
	    }
    	
    }
    
    /*numCurrentCase = 0;
    while ((read = getline(&line, &len, file)) != -1) {
        printf("Retrieved line of length %zu :\n", read);
        printf("%s\n", line);
 
        //int isNumDecide = 0;
        int j = 0;
        for (; j < len; j++){
	    	if (line[j] != ' '){
	    		if (line[j] == '>' || line[j] == '<'){
	    			
	    			/*int k = 0;
	    			for (; k < gridSize * gridSize; k++){
	    				if (cases[k].x == j - 1){
	    				
	    				}
	    					//contraintes
	    			}/*
	    		}
	    		else {
					if (isLineWithNumber) {
						/*CASE thisCase;
						thisCase.x = compt;
						thisCase.y = j / 2;

						if (!isNumDecide){
							int dom[gridSize];
							int i;
							for (i = 0; i < gridSize; i++)
								dom[i] = i + 1;
							thisCase.domaine = dom;
						}
						else {
							int dom[1];
							dom = current;
							thisCase.domaine = dom;
						}/*
							    	
						isLineWithNumber = 0;
						numCurrentCase++;
					}
					else {
				
					  	isLineWithNumber = 1;
					}
				}
			}
        }
        
        compt++;
    }*/
    
    if (line)
        free(line);
        
    return cases;
}

// Penser a tout free parce que j'ai tout compris
int main (int argc, char * argv[]) {
	if (argc - 1 < 1) {
		fprintf(stderr, "Error on argument. Usage :\nfutoshiki <GridfileName>\n");
		exit(EXIT_FAILURE);
	}
	
	FILE *grid = openFile(argv[1]);
	if (grid == NULL)
    {
        fprintf(stderr, "Impossible d'ouvrir le fichier\n");
        exit(EXIT_FAILURE);
	}
	
	CASE * cases = initGrid(grid);
	if (cases == NULL) {
		fprintf(stderr, "Erreur de creation de la grille\n");
        exit(EXIT_FAILURE);
	}
	
	return(0);
}
