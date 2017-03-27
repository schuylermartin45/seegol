/*
** File:    rng.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Simple Random Number Generator
*/
#ifndef _RNG_H_
#define _RNG_H_

/** Headers    **/
#include "gcc16.h"
#include "types.h"

/** Macros     **/

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Intializes the random number generator with a seed
**
** @param seed Seed for the RNG. If 0, initialize with a value derived by the
**        current RTC time
*/
void rng_init(uint16_t seed);

/*
** Intializes the random number generator with an RTC time-based seed
*/
#define rng_init_time() rng_init(0)

/*
** Retrieves a random 16-bit number
**
** @return A random number,/ 16-bits
*/
uint16_t rng_fetch();

/*
** Retrieves a random 16-bit number, within a set range
**
** @param lo Min value in range, inclusive
** @param hi Max value in range, inclusive
** @return A random number,/ 16-bits
*/
uint16_t rng_fetch_range(uint16_t lo, uint16_t hi);

#endif
