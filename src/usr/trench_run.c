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
** Procedurally draw the Death Star trench run
**
** @param seed Seed value to use in rendering
*/
static void __trench_run_render_frame(uint16_t seed)
{
    RGB_8 ROGUE_RED = {200,  44,  32};
    //RGB_8 ROGUE_YLW = {199, 155,  56};
    RGB_8 ROGUE_YLW = {210, 175,  56};
    // calculate values relative to the screen size
    uint16_t fr_w = gl_getw();
    uint16_t fr_h = gl_geth();
    Point_2D ctr_pt = {fr_w / 2, fr_h / 2};
    // box surrounding center point, "openning" to the trench
    Point_2D ctr_ul = {ctr_pt.x - (fr_w / 16), ctr_pt.y - (fr_h / 16)};
    Point_2D ctr_ll = {ctr_pt.x + (fr_w / 16), ctr_pt.y + (fr_h / 16)};

    // "targetting computer" indicator
    gl_draw_strf(PT2(fr_w - (fr_w / 4), fr_h / 20), ROGUE_RED, ROGUE_YLW,
        "|%06d|", &seed, NULL);
    
    // draw the corners of the trenches, based on the screen dimensions
    // top corners of trench
    gl_draw_line(PT2(   0,fr_h / 4), ctr_ul, ROGUE_YLW);
    gl_draw_line(PT2(fr_w,fr_h / 4), PT2(ctr_ll.x, ctr_ul.y), ROGUE_YLW);
    // bottom corners of trench
    gl_draw_line(PT2(   0,fr_h), PT2(ctr_ul.x, ctr_ll.y), ROGUE_YLW);
    gl_draw_line(PT2(fr_w,fr_h), ctr_ll, ROGUE_YLW);

    // bound the end of the trench
    gl_draw_line(ctr_ul, PT2(ctr_ul.x, ctr_ll.y), ROGUE_YLW);
    gl_draw_line(PT2(ctr_ul.x, ctr_ll.y), ctr_ll, ROGUE_YLW);
    gl_draw_line(ctr_ll, PT2(ctr_ll.x, ctr_ul.y), ROGUE_YLW);

    // TODO
    // trench lines determined by seed
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
    __trench_run_render_frame(1342);
    kio_wait_key('q');

    // clean-up and exit
    gl_exit();
    return EXIT_SUCCESS;
}
