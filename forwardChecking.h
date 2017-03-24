#ifndef __FOR_H__
#define __FOR_H__

#include "define.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_FC 0

int forwardChecking(CASE *grid, size_t lineSize, 
	CONTRAINTE *contraintes, size_t nbContraintes);

#endif
