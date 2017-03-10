#ifndef DEFINE_H
#define DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

#define DEBUG_INIT_CONST 0
#define DEBUG_FILE_READING 0

// Nombre attribuer aux domaines interdits
#define NO_DOMAINE -1

#define ERROR -1

enum operateur {
	DIF, SUP, INF
};

typedef struct CASE_ CASE;
typedef struct AFFECTATION_ AFFECTATION;
typedef struct CONTRAINTE_ CONTRAINTE;

struct CASE_ {
	// Index in grid
	size_t ind;
	int * domaine;
	// indices de toutes les contraintes de la case dans le tableau contraintes
	size_t * conts;
	// indice du dernier indice ajouter au tableau precedant
	size_t indLastConst;
	
	// Affectation
	AFFECTATION *affectation;
};

struct AFFECTATION_ {
	CASE * var;
	int * curDomain;
	int curValue;
};

struct CONTRAINTE_ {
	enum operateur op;
	CASE * droite;
	CASE * gauche;
};

#ifdef __cplusplus
}
#endif

#endif
