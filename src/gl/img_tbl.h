/*
** File:    img_tbl.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: File that defines the image look-up table; an array that holds
**              pointers to CXPM image data blocks. This does not define file
**              IDs. These arrays are very large and should only be included
**              once in the project.
*/
#ifndef _GL_IMG_TBL_H_
#define _GL_IMG_TBL_H_

/** Headers    **/
#include "gcc16.h"

// image file ids
#include "img_fids.h"
// image data
#include "../res/img_cxpm/wish_you_were_here_small_8clr_150x100.cxpm"

/** Macros     **/

/** Globals    **/
char** gl_img_tbl[GL_IMG_TBL_SIZE] =
{
    wish_you_were_here_small_8clr_150x100_cxpm,
};

/** Structures **/

/** Functions  **/

#endif
