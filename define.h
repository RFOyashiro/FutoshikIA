#ifndef DEFINE_H
#define DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

// Debugs
#define DEBUG_INIT_CONST 0
#define DEBUG_FILE_READING 0

// Nombre attribuer aux domaines interdits
#define NO_DOMAINE -1

#define ERROR -1

/**
 * The different binary operator of a constraint
 */
enum operateur {
	DIF, SUP, INF
};

typedef struct CASE_ CASE;
typedef struct AFFECTATION_ AFFECTATION;
typedef struct OLD_DOM_ OLD_DOM;
typedef struct CONTRAINTE_ CONTRAINTE;

/**
 * Equivalent to a variable.
 */
struct CASE_ {
	// Index in grid (number of the slot)
	size_t ind;
	// Domain of the variable. All values that it can take
	int * domaine;
	// indices de toutes les contraintes de la case dans le tableau contraintes
	size_t * conts;
	// indice du dernier indice ajouter au tableau precedant
	size_t indLastConst;
	
	// Affectation
	AFFECTATION *affectation;
};

/**
 * An affectation of a variable
 */
struct AFFECTATION_ {
	// The variable that is affected
	CASE * var;
	// number of valid values that this affectation can take
	size_t curDomainSize;
	// The current domain of the slot (we need to not modify the CASE one)
	int * curDomain;
	// The current value affected to the variable
	int curValue;
	
	// Number of domain changed (used for forward checking)
	int compt;
	// Domain before testing values (used for forward checking)
	int *previousDomain;
};

/**
 * Structure to keep trace of domain before modifications (only used in forward checking)
 */
struct OLD_DOM_ {
	// The old value
	int oldDomainValue;
	// The index of the modified value in the domain array
	int indice;
	// The modifier
	AFFECTATION * origin;
};
/**
 * A binary constraint between two variables
 */
struct CONTRAINTE_ {
	// The binary operator of the constraint
	enum operateur op;
	// The left operand
	CASE * droite;
	// The right operand
	CASE * gauche;
};

#ifdef __cplusplus
}
#endif

#endif
