/*
** File:    pane.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Library that provides a common frame in order to put display
**              information on the OS. Some people call these windows. I call
**              them panes
*/
#ifndef _PANE_H_
#define _PANE_H_

/** Headers    **/
#include "../kern/gcc16.h"
#include "../kern/types.h"
#include "../kern/vga/vga.h"
#include "../gl/gl_lib.h"

/** Macros     **/

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Initialize a graphical mode for pane-based programs
**
** @param mode Graphics mode to select
*/
void pane_enter(uint8_t mode);

/*
** Sets the theme for panes. These settings are persistent. If NULL is provided
** for any argument, that value is set to the default color value
**
** @param b_pane Pane background color
** @param f_pane Pane foreground color
** @param b_title Background color of the title bar
** @param f_title Foreground (text) color of the title bar
** @param b_select Background text selection color
** @param f_select Foreground (text) text selection color
** @param text Text color
** @param drop_shadow Boolean controls whether the drop shadow is visible
*/
void pane_set_theme(
    RGB_8* b_pane,   RGB_8* f_pane,
    RGB_8* b_title,  RGB_8* f_title,
    RGB_8* b_select, RGB_8* f_select,
    RGB_8* text,     RGB_8* drop_shadow
);

/*
** Exits a graphical mode. This alias makes more sense when a program only uses
** the Pane library.
*/
#define pane_exit() gl_exit()

/*
** Draws a title pane
**
** @param title Title string
** @param subtitle Subtitle string
*/
void pane_draw_title(char* title, char* subtitle);

/*
** Draws a pane with a title and text
**
** @param title Title string
** @param text Text string
*/
void pane_draw_title_text(char* title, char* text);

/*
** Draws a pane with text
**
** @param text Text string
*/
#define pane_draw_text(text) \
    pane_draw_title_text(NULL, text);

/*
** Draws a pane with a title and an image, centered
**
** @param title Title string
** @param fid File id of the image
*/
void pane_draw_title_img(char* title, uint8_t fid);

/*
** Draws a pane with an image, centered
**
** @param fid File id of the image
*/
#define pane_draw_img(fid) \
    pane_draw_title_img(NULL, fid)

/*
** Draws a pane with a title, an image to the right, and text to the left
**
** @param title Title string
** @param fid File id of the image
** @param text Text to go with the image
*/
void pane_draw_title_img_text(char* title, uint8_t fid, char* text);

/*
** Draws a pane with an image to the right and text to the left
**
** @param fid File id of the image
** @param text Text to go with the image
*/
void pane_draw_img_text(uint8_t fid, char* text);

/*
** Draws a pane with an image to the right and text to the left
**
** @param fid File id of the image
** @param text Text to go with the image
*/
#define pane_draw_img_text(fid, text) \
    pane_draw_title_img_text(NULL, fid, text)

/*
** Draws a prompt pane, which allows the user to pick an option from a menu
**
** @param prompt Prompt string
** @param optc Option count, number of options in the menu
** @param optv Option values, array of strings holding user options
** @return 0-indexed ID of the option selected by the user
*/
uint8_t pane_draw_prompt(char* prompt, uint8_t optc, char* optv[]);

#endif
