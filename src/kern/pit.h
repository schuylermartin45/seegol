/*
** File:    pit.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Programmable Interval Timer driver (Intel 8254)
*/
#ifndef _PIT_H_
#define _PIT_H_

/** Headers    **/
#include "gcc16.h"
#include "types.h"

/** Macros     **/
// clock speed, ticks per second; 1000 = 1ms ticks, 100 = 10ms ticks
#define CLOCK_SPEED 100


/** Globals    **/

// clock value
extern uint16_t pit_clock_value;

/** Functions  **/

/*
** Initializes the clock device
*/
void pit_init(void);

/*
** Clock interrupt service routine
**
** TODO do these matter in Real Mode???
** @param vector interrupt vector
** @param code interrupt code
*/
void pit_isr(void);

#endif
