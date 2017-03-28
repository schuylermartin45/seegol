/*
** File:    rng.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Simple Random Number Generator
*/

/** Headers    **/
#include "gcc16.h"
#include "rng.h"
#include "clock.h"

/** Globals    **/

// RNG is implemented as a Linear Feedback Shift Register (LFSR)
// (I knew that cryptography class would be useful!)
static uint16_t lfsr;
static uint8_t bit;

/*
** Intializes the random number generator with a seed
**
** @param seed Seed for the RNG. If 0, initialize with a value derived by the
**        current RTC time
*/
void rng_init(uint16_t seed)
{
    if (seed == 0)
    {
        RTC_Time t_cur;
        clk_rtc_time(&t_cur);
        // just some arbitrary math I came up with
        lfsr = (t_cur.sec * (t_cur.min % (t_cur.hr + 1))) + t_cur.day;
    }
    else
        lfsr = seed;
}

/*
** Retrieves a random 16-bit number
**
** @return A random number,/ 16-bits
*/
uint16_t rng_fetch()
{
    // polynomial form (Fibonacci LFSR, courtesy of Wikipedia):
    //     x^16 + x^14 + x^13 + x^11 + 1
    // fetch the next bit in the sequence, either a 1 or a 0
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    // feed the result back into the shift register
    return lfsr = (lfsr >> 1) | (bit << 15);
}

/*
** Retrieves a random 16-bit number, within a set range
**
** @param lo Min value in range, inclusive
** @param hi Max value in range, inclusive
** @return A random number,/ 16-bits
*/
uint16_t rng_fetch_range(uint16_t lo, uint16_t hi)
{
    uint16_t ran_num = rng_fetch();
    return (ran_num % (hi - lo)) + lo + 1;
}
