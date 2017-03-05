/*
** File:    trench_run.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: These plans are NOT in the main computer
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "trench_run.h"

#include "../kern/kio.h"
#include "../gl/gl_lib.h"

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void trench_run_init(Program* prog)
{
    prog->name = "trench_run";
    prog->desc = "These plans are NOT in the main computer.";
    prog->usage = "";
    prog->main = &trench_run_main;
}

/*
** Finds a point on a line, given an offset from the front
**
** @param p0 Starting point
** @param p1 Ending point
** @param perc Fraction of the line to find the point (0-10)
** @param out Output point
*/
static void __trench_run_pt_on_line(Point_2D p0, Point_2D p1, uint8_t perc,
    Point_2D* out)
{
    out->x = (p1.x - p0.x) / perc;
    out->y = (p1.y - p0.y) / perc;
}

/*
** Procedurally draw the stars above the trench
**
** @param seed Seed value to use in rendering
** @param tr_ul Upper-left point on trench corner (near plane)
** @param ctr_ul Upper-left point on center corner (far plane)
** @param ctr_ur Upper-right point on center corner (far plane)
** @param tr_ur Upper-right point on trench corner (far plane)
*/
static void __trench_run_render_stars(uint16_t seed,
    Point_2D tr_ul, Point_2D ctr_ul, Point_2D ctr_ur, Point_2D tr_ur)
{
    // manipulate the seed a little bit...because why not?
    seed += seed % 16;
    seed += seed % 10;
    seed += seed % 32;
    // ensure a certain length
    if (seed < 10)
        seed *= 12;
    if (seed < 100)
        seed *= 222;
    if (seed < 1000)
        seed *= 1234;
        
    // use the seed to determine certain parameters
    Point_2D cursor = {seed % gl_getw(), seed % 10};
    uint16_t dx = (seed % 50) + 1;
    uint16_t dy = ((seed % 4) + 1) * 3;

    // counts the cycles
    uint16_t cntr = 0;
    // procedurally draw stars; attempting to make some amount of noise
    // with all the variables at play; stars are drawn starting at the upper
    // left corner and down until we hit a limit
    while (cursor.y < ctr_ul.y)
    {
        // bounds check; fill in the upper trapazoid of space
        if (cursor.y > tr_ul.y)
        {
            // solve the lines
            int16_t ul_dx = ctr_ul.x - tr_ul.x, ul_dy = ctr_ul.y - tr_ul.y;
            int16_t ul_eq = (ul_dx * ctr_ul.x) - (ul_dy * ctr_ul.y);
            int16_t cur_ul_eq = (ul_dx * cursor.x) - (ul_dy * cursor.y);
            int16_t ur_dx = ctr_ur.x - tr_ur.x, ur_dy = ctr_ur.y - tr_ur.y;
            int16_t ur_eq = (ul_dx * ctr_ur.x) - (ul_dy * ctr_ur.y);
            int16_t cur_ur_eq = (ur_dx * cursor.x) - (ur_dy * cursor.y);
            // see if we should draw this
            if ((cur_ul_eq > ul_eq) && (cur_ur_eq > ur_eq))
                gl_put_pixel(cursor, RGB_WHITE);
        }
        else
                gl_put_pixel(cursor, RGB_WHITE);
        switch (cntr % 4)
        {
            case 0: cursor.x = cursor.x + dx;
                break;
            case 1: cursor.x = cursor.x + (((cntr % 2) + 1) * dx);
                break;
            case 2: cursor.x = cursor.x + (dy * dx);
                break;
            case 3: cursor.x = cursor.x + (dy / 3) + (((dx * dy) % cntr));
                break;
        }
        if (cursor.x >= gl_getw())
        {
            cursor.x %= gl_getw();
            if (cntr % 2)
                cursor.y += dy;
            else
                cursor.y += (dy * 2) + 1;
        }
        ++cntr;
    }
}

/*
** Procedurally draw the Death Star trench run
**
** @param seed Seed value to use in rendering
*/
static void __trench_run_render_frame(uint16_t seed)
{
    RGB_8 ROGUE_RED = {200,  44,  32};
    RGB_8 ROGUE_YLW = {210, 175,  56};
    // calculate values relative to the screen size
    uint16_t fr_w = gl_getw();
    uint16_t fr_h = gl_geth();
    Point_2D ctr_pt = {fr_w / 2, fr_h / 2};
    // box surrounding near plane of the image, the start of the corner lines
    Point_2D tr_ul = {0, fr_h / 4};
    Point_2D tr_ll = {fr_w, fr_h};
    // box surrounding center point, "openning" to the trench (far plane)
    Point_2D ctr_ul = {ctr_pt.x - (fr_w / 16), ctr_pt.y - (fr_h / 16)};
    Point_2D ctr_ll = {ctr_pt.x + (fr_w / 16), ctr_pt.y + (fr_h / 16)};

    // generate stars
    __trench_run_render_stars(seed, tr_ul, ctr_ul,
        PT2(ctr_ll.x, ctr_ul.y), PT2(tr_ul.x, tr_ul.y));

    // "targetting computer" indicator
    gl_draw_strf(PT2(fr_w - (fr_w / 4), fr_h / 20), ROGUE_RED, ROGUE_YLW,
        "|%06d|", &seed, NULL);
    
    // draw the corners of the trenches, based on the screen dimensions
    // top corners of trench
    gl_draw_line(tr_ul, ctr_ul, ROGUE_YLW);
    gl_draw_line(PT2(tr_ll.x, tr_ul.y), PT2(ctr_ll.x, ctr_ul.y), ROGUE_YLW);
    // bottom corners of trench
    gl_draw_line(PT2(tr_ul.x, tr_ll.y), PT2(ctr_ul.x, ctr_ll.y), ROGUE_YLW);
    gl_draw_line(tr_ll, ctr_ll, ROGUE_YLW);

    // bound the end of the trench
    gl_draw_line(ctr_ul, PT2(ctr_ul.x, ctr_ll.y), ROGUE_YLW);
    gl_draw_line(PT2(ctr_ul.x, ctr_ll.y), ctr_ll, ROGUE_YLW);
    gl_draw_line(ctr_ll, PT2(ctr_ll.x, ctr_ul.y), ROGUE_YLW);

    // TODO
    // trench lines determined by seed
    Point_2D tr_p0;
    Point_2D tr_p1;
    __trench_run_pt_on_line(tr_ul, ctr_ul, 5, &tr_p0);
    __trench_run_pt_on_line(PT2(tr_ul.x, tr_ll.y), PT2(ctr_ul.x, ctr_ul.y),
        5, &tr_p1);
    gl_draw_line(tr_p0, tr_p1, ROGUE_YLW);
    __trench_run_pt_on_line(ctr_ll, tr_ll, 5, &tr_p0);
    gl_draw_line(tr_p1, tr_p0, ROGUE_YLW);
}

/*
** Main method for program
*/
uint8_t trench_run_main(uint8_t argc, char* argv[])
{
    gl_init();
    // default to VGA13
    gl_enter(VGA_MODE_13);

    // TODO animate!
    // draw the plans w/ a seed
    for(uint16_t t=0; t<1000; ++t)
    {
        __trench_run_render_frame(1342 + t);
        // TODO get a real timer
        //kio_wait_key(KEY_SPACE);
        uint16_t time_block = 0;
        for(uint16_t s=0; s<65000; ++s)
            time_block += s;
        gl_clrscr();
        for(uint16_t s=0; s<65000; ++s)
            time_block += s;
    }

    // clean-up and exit
    gl_exit();
    return EXIT_SUCCESS;
}
