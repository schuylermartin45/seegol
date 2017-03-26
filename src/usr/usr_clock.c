/*
** File:    usr_clock.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Displays time for the user
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "usr_clock.h"

#include "../kern/clock.h"
#include "../kern/kio.h"

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void usr_clock_init(Program* prog)
{
    prog->name = "clock";
    prog->desc =
        "Displays the current system time. 'q' to quit.";
    prog->usage = "";
    prog->main = &usr_clock_main;
}

/*
** Main method for the program
*/
uint8_t usr_clock_main(uint8_t argc, char* argv[])
{
    // time tracking data
    char t_buff[RTC_STR_BUFF_SIZE];
    RTC_Time t_cur, t_prev = {0, 0, 0};
    kio_clrscr();

    char key = '\0';
    do
    {
        // current time stamp
        clk_rtc_time(&t_cur);
        if (clk_rtc_diff(t_cur, t_prev))
        {
            kio_clrscr();
            clk_rtc_str(t_buff, t_cur, false);
            kio_printf(
                "         "
                "Ticking away, the moments that makes up the dull day..."
                "\n                     %s\n", t_buff, NULL
            );
            t_prev = t_cur;
        }
        // non-blocking get chr
        key = kio_getchr_nb();
    }
    while (key != 'q');
    // capture excess q from non-blocking call; otherwise this gets dumped
    // to seesh. Also clear the screen
    kio_clrscr();

    return EXIT_SUCCESS;
}
