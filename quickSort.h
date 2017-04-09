/* 
 * File:   quickSort.h
 */

#ifndef __QUICKSORT_H__
#define __QUICKSORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "define.h"
	
/**
 * \fn void quickSortMain (AFFECTATION **tab[], const size_t tabSize)
 * \brief Sort an AFFECTATION array by domain size
 * \param[in, out] tab The array to sort. All tabSize values must be initialized.
 * \param[in] tabSize the size of tab.
 */
void quickSortMain (AFFECTATION **tab, const size_t tabSize);

#ifdef __cplusplus
}
#endif

#endif /* __QUICKSORT_H__ */

