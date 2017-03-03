/*
** File:    img_fids.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: File that defines the image look-up table keys. These are file
**              IDs available to the user. This prevents the user program from
**              accessing the image file data directly, since it is large.
*/
#ifndef _GL_IMG_FIDS_H_
#define _GL_IMG_FIDS_H_

/** Headers    **/
#include "gcc16.h"

/** Macros     **/
// size of the image table
#define GL_IMG_TBL_SIZE  2

// file IDs, keys into the table
// TODO not enough space for some images
#define IMG_FID_HSC      0   // Original HSC Logo
#define IMG_FID_DSTM     1   // Dark Side of the Moon
#define IMG_FID_WYWH     2   // Wish You Were Here

/** Globals    **/

/** Structures **/

/** Functions  **/

#endif
