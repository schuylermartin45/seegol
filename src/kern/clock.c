/*
** File:    clock.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Clock control:
**              1) Real Time Clock - RTC
*/

/** Headers    **/
#include "gcc16.h"
#include "clock.h"

#include "asm_lib.h"
#include "kio.h"

/***** Real Time Clock (RTC) Functions               *****/

/*
** Check if the clock
**
** @return If the RTC is in the middle of an update
*/
bool __clk_rtc_chk_update_flag()
{
    _outb(CMOS_ADDR, 0x0A);
    return _inb(CMOS_DATA) & 0x80;
}

/*
** Fetch information from the CMOS registers
**
** @param rtc_reg RTC Register
** @return Value of the RTC register value in question
*/
uint8_t __clk_rtc_get_reg(uint8_t reg)
{
    _outb(CMOS_ADDR, reg);
    return _inb(CMOS_DATA);
}

/*
** Fetches the current RTC time
**
** @param time Time struct to set
*/
void clk_rtc_time(RTC_Time *time)
{
    // this is the lazy man's approach to dealing with the fact that the RTC
    // device takes time to update time
    while (__clk_rtc_chk_update_flag()) {}

    time->sec = __clk_rtc_get_reg(0x00);
    time->min = __clk_rtc_get_reg(0x02);
    time->hr  = __clk_rtc_get_reg(0x04);
    time->day = __clk_rtc_get_reg(0x07);
    time->mon = __clk_rtc_get_reg(0x08);
    time->yr  = __clk_rtc_get_reg(0x09);

    // adjust time information when CMOS register B is set to "BCD" mode
    // this gets us the correct human-readable display time
    uint8_t reg_b = __clk_rtc_get_reg(0x0B);
    if (!(reg_b & 0x04))
    {
        // hr BCD calculation is different from the others...for some reason
        time->hr  = ((time->hr & 0x0F) +
            (((time->hr & 0x70) / 16) * 10) ) | (time->hr & 0x80);
        time->min = (time->min & 0x0F) + ((time->min / 16) * 10);
        time->sec = (time->sec & 0x0F) + ((time->sec / 16) * 10);
        time->day = (time->day & 0x0F) + ((time->day / 16) * 10);
        time->mon = (time->mon & 0x0F) + ((time->mon / 16) * 10);
        time->yr  = (time->yr  & 0x0F) + ((time->yr  / 16) * 10);
    }
    // 12 -> 24 hour clock check
    if (!(reg_b & 0x02) && (time->hr & 0x80))
        time->hr = ((time->hr & 0x7F) + 12) % 24;
}

/*
** Converts an RTC time struct to a string
**
** @param buff String to fill
** @param time RTC Time structure to set
** @param is_24 Flag for 24 formating. True for 24hr mode, False for AM/PM
*/
void clk_rtc_str(char* buff, RTC_Time time, bool is_24)
{
    uint8_t hr = time.hr;
    char* am_pm = "";
    if (!is_24)
    {
        if (hr > 12)
        {
            am_pm = "PM";
            hr -= 12;
        }
        else
        {
            if (hr == 0)
                hr = 12;
            am_pm = "AM";
        }
    }
    // date
    kio_sprintf("%02D/%02D/", buff, &(time.mon), &(time.day));
    buff += 6;
    kio_sprintf("%02D ", buff, &(time.yr), NULL);
    buff += 3;
    // time
    kio_sprintf("%02D:%02D:", buff, &hr, &(time.min));
    buff += 6;
    kio_sprintf("%02D %2s", buff, &(time.sec), am_pm);
}
