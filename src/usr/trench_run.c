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

#include "../kern/clock.h"
#include "../kern/kio.h"
#include "../kern/rng.h"
#include "../gl/gl_lib.h"
#include "../gl/pane.h"

/** Macros     **/
// trench run colors
#define ROGUE_RED       RGB(200,  44,  32)
#define ROGUE_YLW       RGB(210, 175,  56)
#define ROGUE_BLK       RGB( 80,  80,  80)
// Trench run length
#define DEFAULT_RUN_LEN 30
// infinite drawing mode
#define INFINITE_MODE   255
// chances of drawing a star
#define STAR_PROB_LO    0
#define STAR_PROB_HI    100
#define STAR_PROB       77  // year the movie came out
// padding on targeting computer border
#define TCB_PAD         1

/*
** Initializes program structure
**
** @param prog Program pointer to set
*/
void trench_run_init(Program* prog)
{
    prog->name = "Trench_Run";
    prog->desc = "These plans are NOT in the main computer.";
    prog->usage = "";
    prog->main = &trench_run_main;
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
    // ensure a certain length
    if (seed < 10)
        seed *= 12;
    if (seed < 100)
        seed *= 222;
    if (seed < 1000)
        seed *= 1234;
        
    // use the seed to determine certain parameters
    Point_2D cursor = {seed % gl_getw(), seed % 10};
    uint16_t dx = (seed % 35) + 1;
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
            // Form of equation in use: 2 point form
            // x(dy) - y(dx) = x1y2 - x2y1; dy = y2-y1; dx = x2 - x1
            // Upper-left top edge of the trench
            int16_t ul_dx = ctr_ul.x - tr_ul.x, ul_dy = ctr_ul.y - tr_ul.y;
            int16_t ul_eq = (tr_ul.x * ctr_ul.y) - (ctr_ul.x * tr_ul.y);
            int16_t cur_ul_eq = (cursor.x * ul_dy) - (cursor.y * ul_dx);
            // Upper-right top edge of the trench
            int16_t ur_dx = ctr_ur.x - tr_ur.x, ur_dy = ctr_ur.y - tr_ur.y;
            int16_t ur_eq = (tr_ur.x * ctr_ur.y) - (ctr_ur.x * tr_ur.y);
            int16_t cur_ur_eq = (cursor.x * ur_dy) - (cursor.y * ur_dx);
            // see if we should draw this
            if ((cur_ul_eq > ul_eq) && (cur_ur_eq < ur_eq))
            {
                if (rng_fetch_range(STAR_PROB_LO, STAR_PROB_HI) > STAR_PROB)
                    gl_put_pixel(cursor, RGB_WHITE);
            }
        }
        else
        {
            if (rng_fetch_range(STAR_PROB_LO, STAR_PROB_HI) > STAR_PROB)
                gl_put_pixel(cursor, RGB_WHITE);
        }
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
    // calculate values relative to the screen size
    uint16_t fr_w = gl_getw();
    uint16_t fr_h = gl_geth();
    Point_2D ctr_pt = {fr_w / 2, fr_h / 2};
    // box surrounding near plane of the image, the start of the corner lines
    Point_2D tr_ul = {0, fr_h / 4};
    Point_2D tr_lr = {fr_w, fr_h};
    // box surrounding center point, "openning" to the trench (far plane)
    Point_2D ctr_ul = {ctr_pt.x - (fr_w / 16), ctr_pt.y - (fr_h / 16)};
    Point_2D ctr_lr = {ctr_pt.x + (fr_w / 16), ctr_pt.y + (fr_h / 16)};

    // generate stars
    __trench_run_render_stars(seed, tr_ul, ctr_ul,
        PT2(ctr_lr.x, ctr_ul.y), PT2(tr_ul.x, tr_ul.y));

    // "targetting computer" indicator, lower and center just like the movie
    Point_2D tc_ul = {0, 0};
    Point_2D tc_bb;
    gl_draw_strf_bb(tc_ul, "%06d", 2, fr_w, &tc_bb, &seed, NULL);
    tc_ul.x = (fr_w - tc_bb.x) / 2;
    tc_ul.y = fr_h - (tc_bb.y + (tc_bb.y / 2));
    gl_draw_strf_scale(tc_ul, ROGUE_RED, ROGUE_RED, "%06d", 2, fr_w, &seed,
        NULL);

    // draw frame around the letters
    Point_2D tcb_ul = {tc_ul.x - TCB_PAD, tc_ul.y - TCB_PAD};
    Point_2D tcb_lr = {
        tc_ul.x + tc_bb.x + TCB_PAD,
        tc_ul.y + tc_bb.y + TCB_PAD
    };
    gl_draw_line(tcb_ul, PT2(tcb_lr.x, tcb_ul.y), ROGUE_YLW);
    gl_draw_line(tcb_ul, PT2(tcb_ul.x, tcb_lr.y), ROGUE_YLW);
    gl_draw_line(PT2(tcb_ul.x, tcb_lr.y), tcb_lr, ROGUE_YLW);
    gl_draw_line(PT2(tcb_lr.x, tcb_ul.y), tcb_lr, ROGUE_YLW);

    // draw the corners of the trenches, based on the screen dimensions
    // top corners of trench
    gl_draw_line(tr_ul, ctr_ul, ROGUE_YLW);
    gl_draw_line(PT2(tr_lr.x, tr_ul.y), PT2(ctr_lr.x, ctr_ul.y), ROGUE_YLW);
    // bottom corners of trench
    gl_draw_line(PT2(tr_ul.x, tr_lr.y), PT2(ctr_ul.x, ctr_lr.y), ROGUE_YLW);
    gl_draw_line(tr_lr, ctr_lr, ROGUE_YLW);
    // y values of upper middle stripe
    uint16_t tr_md_u_y  = tr_ul.y  + ((tr_lr.y - tr_ul.y)   / 4);
    uint16_t ctr_md_u_y = ctr_ul.y + ((ctr_lr.y - ctr_ul.y) / 4);
    // y values of middle middle stripe
    uint16_t tr_md_m_y  = tr_ul.y  + ((tr_lr.y - tr_ul.y)   / 2);
    uint16_t ctr_md_m_y = ctr_ul.y + ((ctr_lr.y - ctr_ul.y) / 2);
    // y values of lower middle stripe
    uint16_t tr_md_l_y  = tr_ul.y  + ((tr_lr.y - tr_ul.y)   * 3 / 4);
    uint16_t ctr_md_l_y = ctr_ul.y + ((ctr_lr.y - ctr_ul.y) * 3 / 4);
    // middle lines, left side, upper to bottom
    gl_draw_line(
        PT2(tr_ul.x,  tr_md_u_y), PT2(ctr_ul.x, ctr_md_u_y), ROGUE_YLW
    );
    gl_draw_line(
        PT2(tr_ul.x,  tr_md_m_y), PT2(ctr_ul.x, ctr_md_m_y), ROGUE_YLW
    );
    gl_draw_line(
        PT2(tr_ul.x,  tr_md_l_y), PT2(ctr_ul.x, ctr_md_l_y), ROGUE_YLW
    );
    // middle lines, right side, upper to bottom
    gl_draw_line(
        PT2(tr_lr.x,  tr_md_u_y), PT2(ctr_lr.x, ctr_md_u_y), ROGUE_YLW
    );
    gl_draw_line(
        PT2(tr_lr.x,  tr_md_m_y), PT2(ctr_lr.x, ctr_md_m_y), ROGUE_YLW
    );
    gl_draw_line(
        PT2(tr_lr.x,  tr_md_l_y), PT2(ctr_lr.x, ctr_md_l_y), ROGUE_YLW
    );

    // bound the end of the trench
    gl_draw_line(ctr_ul, PT2(ctr_ul.x, ctr_lr.y), ROGUE_YLW);
    gl_draw_line(PT2(ctr_ul.x, ctr_lr.y), ctr_lr, ROGUE_YLW);
    gl_draw_line(ctr_lr, PT2(ctr_lr.x, ctr_ul.y), ROGUE_YLW);

    // trench lines determined by seed
    uint16_t line_arr[3];
    switch (seed % 3)
    {
        case 0:
            line_arr[0] = (fr_w / 32) * 7;
            line_arr[1] = (fr_w / 32) * 10;
            line_arr[2] = (fr_w / 32) * 13;
            break;
        case 1:
            line_arr[0] = 0;
            line_arr[1] = (fr_w / 32) * 8;
            line_arr[2] = (fr_w / 32) * 12;
            break;
        case 2:
            line_arr[0] = (fr_w / 32) * 6;
            line_arr[1] = 0;
            line_arr[2] = (fr_w / 32) * 11;
            break;
    }
    for(uint8_t tr_lines=0; tr_lines<3; ++tr_lines)
    {
        uint16_t frac_w = line_arr[tr_lines];
        // skip line
        if (frac_w == 0)
            continue;
        Point_2D tr_p0 = {frac_w, 0};
        Point_2D tr_p1 = tr_p0;

        // cheaty way; solving equations without decimals is hard
        // ...high school geometry is also hard, for the record
        RGB_8 cmp_color = {0,0,0};
        // find the first 2 yellow pixels to draw the left vertical bar
        uint16_t y = 0;
        for(; y<fr_h; ++y)
        {
            tr_p0.y = y;
            gl_get_pixel(tr_p0, &cmp_color);
            if (vga_RGB_8_cmp(cmp_color, ROGUE_YLW))
            {
                ++y;
                break;
            }
        }
        // ignore the middle lines
        uint8_t ylw_cntr = 0;
        for(; y<fr_h; ++y)
        {
            tr_p1.y = y;
            gl_get_pixel(tr_p1, &cmp_color);
            if (vga_RGB_8_cmp(cmp_color, ROGUE_YLW))
            {
                ++ylw_cntr;
                if (ylw_cntr > 3)
                    break;
            }
        }
        gl_draw_line(tr_p0, tr_p1, ROGUE_YLW);
        // finding the subsequent bars is easier since we know one dimension
        // horizontal bar
        tr_p0 = tr_p1;
        // start looking for x past the center of the screen; this prevents
        // issues with running into aliased flat lines; "the jaggies"
        bool edge_found = false;
        for(uint16_t x=(fr_w / 2)+1; x<fr_w; ++x)
        {
            tr_p0.x = x;
            gl_get_pixel(tr_p0, &cmp_color);
            if (vga_RGB_8_cmp(cmp_color, ROGUE_YLW))
                edge_found = true;
            if (edge_found && !vga_RGB_8_cmp(cmp_color, ROGUE_YLW))
            {
                tr_p0.x = x - 1;
                break;
            }
        }
        // handle the aliasing on the other side
        gl_draw_line(tr_p1, tr_p0, ROGUE_YLW);
        // last vertical bar, built just like the first one
        tr_p1 = tr_p0;
        y = tr_p1.y;
        ylw_cntr = 0;
        for(; y<fr_h; --y)
        {
            tr_p1.y = y;
            gl_get_pixel(tr_p1, &cmp_color);
            if (vga_RGB_8_cmp(cmp_color, ROGUE_YLW))
            {
                ++ylw_cntr;
                if (ylw_cntr > 4)
                    break;
            }
        }
        gl_draw_line(tr_p0, tr_p1, ROGUE_YLW);
    }
}

/*
** Main method for program
*/
uint8_t trench_run_main(uint8_t argc, char* argv[])
{
    // default to VGA13
    pane_enter(VGA_MODE_13);
    // set a theme that matches the look of the trench-run program
    pane_set_theme(
        &ROGUE_BLK, &RGB_BLACK,
        &ROGUE_RED, &ROGUE_YLW,
        &ROGUE_YLW, &ROGUE_RED,
        &ROGUE_YLW, &ROGUE_BLK
    );

    // run length and seed for the trench
    uint8_t run_len = DEFAULT_RUN_LEN;
    uint16_t seed = 0;
    // option menu for trench run simulations settings
    char* opts[] =
    {
        "Infinite trench run, seed 0",
        "Trench run, seed 0",
        "Trench run, seed 1342",
        "Quit",
    };
    uint8_t opt = pane_draw_prompt("Pick a simulation", 4, opts);
    switch (opt)
    {
        case 0:
            run_len = INFINITE_MODE;
            break;
        case 2:
            seed = 1342;
            break;
        case 3:
            // quit by forcing the loop to not execute
            run_len = 0;
            break;
    }

    RTC_Time t_cur, t_prev = {0, 0, 0};
    // control the frame to draw
    uint16_t fr = 0, cmp_fr= 0;
    char key = '\0';
    // draw the plans w/ a seed; star placement is determined by an RNG
    rng_init(seed);
    while((key != 'q') && (cmp_fr < run_len))
    {
        // animation control w/ timer
        clk_rtc_time(&t_cur);
        if (clk_rtc_diff(t_cur, t_prev))
        {
            gl_clrscr();
            __trench_run_render_frame(seed + fr);
            t_prev = t_cur;
            ++fr;
            // allows us to have an infinite draw scheme
            if (run_len != INFINITE_MODE)
                ++cmp_fr;
        }
        // non-blocking get chr
        key = kio_getchr_nb();
    }

    // clean-up and exit
    pane_exit();
    return EXIT_SUCCESS;
}
