#include <unistd.h>
#include <stdio.h>

enum operateur {DIF, SUP, INF};

struct {
	int ligne;
	int colone;
	int domaine[];
	int adjacences[]; //indices de toutes les contraintes de la case
} CASE

struct {
	operateur op;
	CASE * elementDroite;
	CASE * elementGauche;
} CONTRAINTE

CONTRAINTE contraintes[];

FILE * OpenFile (char * fileName) {
	FILE * grille = fopen(fileName, "r");
	
	if (grille == NULL)
    {
        printf("Impossible d'ouvrir le fichier\n");
        exit(-1);
	}
	
	return grille;
}

void CloseFile (FILE * file) {
	fclose(file);
}

int main (int argc, char * argv[]) {
	
}
