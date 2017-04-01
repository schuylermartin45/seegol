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
#include "../gl/gl_lib.h"
#include "../gl/img_fids.h"

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void usr_clock_init(Program* prog)
{
    prog->name = "Clock";
    prog->desc =
        "Displays the current system time. 'q' to quit. '-t' for text clock.";
    prog->usage = "-t";
    prog->main = &usr_clock_main;
}

/*
** Draws an arm of a clock, following a square frame
** For many reasons, using a circle model (which keeps the arm lengths
** consistent in size) is not possible nor practical.
**
** @param clk_center Center point of the clock
** @param width Clock arm width
** @param color Color of the clock arm
** @param divisions Number of divisions in this time unit (i.e. seconds have
**        60 divisions in a clock)
** @param rad Arm radius. The bounding box of the clock is 2 radii wide
** @param t_unit Time unit value of the arm
*/
static void __usr_clock_draw_arm(Point_2D clk_center, uint8_t width,
    RGB_8 color, uint16_t divisions, uint16_t rad, uint8_t t_unit)
{
    // assume the end point of the arm starts at 12 o'clock position
    Point_2D end_pt = clk_center;
    // divisions of time in an octant
    uint16_t oct_div = (divisions / 8) + 1;
    // 1 unit of change in the sampling of an octant
    uint16_t rad_frac = rad / oct_div;
    // how far along a square's edge we travel, based on time
    uint16_t travel = (t_unit % oct_div) * rad_frac;
    // the error is much higher for the hour hand b/c there are far fewer
    // divisions. This hack seems to correct the error and produce hour hands
    // that are "good enough"
    if (divisions == 12)
    {
        travel = ((t_unit % oct_div) + 1) * rad_frac;
        if (t_unit == 6)
        {
            travel = rad;
            t_unit = 7;
        }
        else if (t_unit > 6)
            t_unit += 2;
    }
    // a smaller error correction is made for the other two hands
    else if (t_unit >= 30)
        travel += (2 * rad_frac);
    // determine the octant we're in
    if      (t_unit < (1 * oct_div))   // 1st
    {
        end_pt.x += travel;
        end_pt.y -= rad;
    }
    else if (t_unit < (2 * oct_div))   // 2nd
    {
        end_pt.x += rad;
        end_pt.y = (end_pt.y - rad) + travel;
    }
    else if (t_unit < (3 * oct_div))   // 3rd
    {
        end_pt.x += rad;
        end_pt.y += travel;
    }
    else if (t_unit < (4 * oct_div))   // 4th
    {
        end_pt.x += rad - travel;
        end_pt.y += rad;
    }
    else if (t_unit < (5 * oct_div))   // 5th
    {
        end_pt.x -= travel;
        end_pt.y += rad;
    }
    else if (t_unit < (6 * oct_div))   // 6th
    {
        end_pt.x -= rad;
        end_pt.y += rad - travel;
    }
    else if (t_unit < (7 * oct_div))   // 7th
    {
        end_pt.x -= rad;
        end_pt.y -= travel;
    }
    else //if (t_unit <= (8 * oct_div))    // 8th
    {
        end_pt.x -= rad - travel;
        end_pt.y -= rad;
    }
    // draw a line from the center of the circle to the end of the arm    
    gl_draw_line_width(clk_center, end_pt, width, color);
}

/*
** Renders a frame of the GUI clock
**
** @param t Current time
** @param t_str Time in string form
*/
static void __usr_clock_render_gui(RTC_Time t, char* t_str)
{
    gl_clrscr();
    // draw an appropriate background
    gl_draw_img_center_scale(IMG_FID_DSTM, 3);

    // draw digital clock, which looks cool, top & centered
    Point_2D digi_bb;
    gl_draw_str_bb(PT2(0, 0), t_str, 1, gl_getw(), &digi_bb);
    Point_2D digi_ul = {(gl_getw() - digi_bb.x) / 2, digi_bb.y};
    gl_draw_str(digi_ul, RGB_WHITE, RGB_WHITE, t_str);

    // draw three clock arms based on the current time
    Point_2D clk_center = {gl_getw() / 2, gl_geth() / 2};
    // draw fastest moving to slowest moving (longest arm to shortest) in case
    // of any overlap
    // sec
    __usr_clock_draw_arm(clk_center, 1, RGB_YELLOW, 60, gl_getw() / 6, t.sec);
    // min
    __usr_clock_draw_arm(clk_center, 2, RGB_CYAN, 60, gl_getw() / 9, t.min);
    // hr
    uint8_t hr = (t.hr > 12) ? t.hr - 12 : t.hr;
    __usr_clock_draw_arm(clk_center, 3, RGB_MAGENTA, 12, gl_getw() / 18, hr);
}

/*
** Renders a frame of the text clock
**
** @param t Current time
** @param t_str Time in string form
*/
static void __usr_clock_render_txt(RTC_Time t, char* t_str)
{
    kio_clrscr();
    kio_printf(
        "         "
        "Ticking away, the moments that makes up the dull day..."
        "\n                     %s\n", t_str, NULL
    );
}

/*
** Main method for the program
*/
uint8_t usr_clock_main(uint8_t argc, char* argv[])
{
    // mode selection and usage check
    bool gui_mode = true;
    if ((argc == 2) && (kio_strcmp(argv[1], "-t")))
    {
        gui_mode = false;
        kio_clrscr();
    }
    else if (argc == 1)
        gl_enter(VGA_MODE_13);
    else
        return ERR_PROG_USAGE;

    // time tracking data
    RTC_Time t_cur, t_prev = {0, 0, 0};
    char t_buff[RTC_STR_BUFF_SIZE];
    char key = '\0';
    do
    {
        // current time stamp
        clk_rtc_time(&t_cur);
        if (clk_rtc_diff(t_cur, t_prev))
        {
            clk_rtc_str(t_buff, t_cur, false);
            if (gui_mode)
                __usr_clock_render_gui(t_cur, t_buff);
            else
                __usr_clock_render_txt(t_cur, t_buff);
            t_prev = t_cur;
        }
        // non-blocking get chr
        key = kio_getchr_nb();
    }
    while (key != 'q');

    // clean-up
    if (gui_mode)
        gl_exit();
    else
        kio_clrscr();

    return EXIT_SUCCESS;
}
