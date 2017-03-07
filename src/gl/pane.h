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

/** Macros     **/

/** Globals    **/

/** Structures **/

/** Functions  **/

/*
** Enter a graphical mode for pane-based programs
**
** @param mode Graphics mode to select
*/
void pane_enter(uint8_t mode);

/*
** Exit graphical pane mode and go back to text
*/
void pane_exit(void);

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
void pane_draw_text(char* text);

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
void pane_draw_img(uint8_t fid);

#endif
