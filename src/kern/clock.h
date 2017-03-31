/*
** File:    clock.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Clock control:
**              1) Real Time Clock - RTC
*/
#ifndef _CLOCK_H_
#define _CLOCK_H_

/** Headers    **/
#include "gcc16.h"
#include "types.h"

/** Macros     **/
// CMOS Port registers 
#define CMOS_ADDR   0x70
#define CMOS_DATA   0x71
// size of the string buffer to use when printing time data
#define RTC_STR_BUFF_SIZE   21

/** Globals    **/

/** Structures **/
// "Ticking away, the moments that make up a dull day..."
// Real Time Clock time information
typedef struct RTC_Time
{
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
    uint8_t day;
    uint8_t mon;
    uint8_t yr;
} RTC_Time;

/** Functions  **/

/***** Real Time Clock (RTC) Functions               *****/

/*
** Fetches the current RTC time
**
** @param time Time struct to set
*/
void clk_rtc_time(RTC_Time* time);

/*
** Converts an RTC time struct to a string
**
** @param buff String to fill
** @param time RTC Time structure to set
** @param is_24 Flag for 24 formating. True for 24hr mode, False for AM/PM
*/
void clk_rtc_str(char* buff, RTC_Time time, bool is_24);

/*
** Determines if two times are different
**
** @param t0 First time to check
** @param t1 Second time to check
** @return True if they are the same time, False otherwise
*/
#define clk_rtc_diff(t0, t1) \
    (t0.sec != t1.sec) || (t0.min != t1.min) || (t0.hr != t1.hr)

/*
** Simple clock busy wait for relatively short amounts of time
**
** @param delay Delay in seconds
*/
void clk_busy_wait(uint8_t delay);

#endif
