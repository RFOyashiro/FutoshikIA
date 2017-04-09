/**
 * \file quickSort.c
 * \brief Sort algorithim using QuickSort.
 */

#include "quickSort.h"
#include "define.h"
#include "stdio.h"

#include <stdlib.h>

/**
 * \fn void swap(AFFECTATION **tab[], size_t x, size_t y)
 * \brief Swap two value in an array of TYPE.
 * \param[in, out] tab The array to swap into. Must be initialized and contains valids values of index x and y.
 * \param[in] x The index of the first value. Must be a valid index of tab.
 * \param[in] y The index of the second value. Must be a valide index of tab.
 */
void swap(AFFECTATION **tab, const size_t x, const size_t y) {
	AFFECTATION *tmp = tab[x];
	tab[x] = tab[y];
	tab[y] = tmp;
}

/**
 * \fn void putPivot (AFFECTATION **tab[], const size_t left, const size_t right, size_t *pivot)
 * \brief Place the pivot at the right place in the provided array 
 * 		  so that all values on the "left" are below to the pivot and all values on the "right" are above.
 * \param[in, out] tab The array to modify. All values from left to right must be initialized.
 * \param[in] left The current minimum index of tab. Must be a valid index of tab
 * \param[in] right The current maximum index of tab. Must be a valide index of tab.
 * \param[in] pivot A pointer to the current index of the pivot. Must point to a valid index of tab.
 */
void putPivot(AFFECTATION **tab, const size_t left, const size_t right, size_t *pivot) {
	*pivot = left;

	for (size_t i = left + 1; i <= right; ++i) {
		AFFECTATION *curAff = tab[i];
		AFFECTATION *leftAff = tab[left];

		if (curAff->curDomainSize < leftAff->curDomainSize) {

			++(*pivot);
			swap(tab, i, *pivot);
		}
	}

	swap(tab, left, *pivot);
}

/**
 * \fn void quickSort (AFFECTATION **ab[], const size_t left, const size_t right)
 * \brief Sort an array with the quicksort algorithim.
 * \param[in, out] tab The array to sort. All values from left to right must be initialized.
 * \param[in] left The current minimum index of tab. Must be a valid index of tab
 * \param[in] right The current maximum index of tab. Must be a valide index of tab.
 */
void quickSort(AFFECTATION **tab, const size_t left, const size_t right) {
	if (left < right) {
		size_t pivot;
		putPivot(tab, left, right, &pivot);
		if (pivot - left >= 1) {
			quickSort(tab, left, pivot - 1);
			quickSort(tab, pivot + 1, right);
		}
	}
}

/**
 * \fn void quickSortMain (AFFECTATION **tab[], const size_t tabSize)
 * \brief Sort an AFFECTATION array by domain size
 * \param[in, out] tab The array to sort. All tabSize values must be initialized.
 * \param[in] tabSize the size of tab.
 */
void quickSortMain (AFFECTATION **tab, const size_t tabSize) {
	quickSort(tab, 0, tabSize - 1);
}