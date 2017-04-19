
#ifndef __TIMER_H__
#define __TIMER_H__ 

#include <time.h>

/**
 * \fn getTimeElapsedInMilliseconds()
 * \author Julien TEULLE
 * \brief Calculate the time elapsed between begin and end in 
 * milliseconds.
 * \return The value of this time
 */
void getTimeElapsed(clock_t begin, clock_t end, int Result[]);

/**
 * \fn CalcTotalTime (double Time, int Result[])
 * \author Kurt SAVIO
 * \brief Convert a time in milliseconds to minute, seconds and milliseconds
 * \param Time : a time in milliseconds
 * \param Result[] : an array in which one minute, second and milliseconds will be stocked
 */
void CalcTotalTime (double Time, int Result[]);

#endif
