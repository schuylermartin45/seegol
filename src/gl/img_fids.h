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
#define GL_IMG_TBL_SIZE      2

// file IDs, keys into the table
// definition is used to conditionally add these images; there's not enough
// space to add all of them
#ifdef GL_IMG_TBL_SIZE
    #define IMG_FID_HSC      0   // Original HSC Logo
#endif
#if GL_IMG_TBL_SIZE > 1
    #define IMG_FID_DSTM     1   // Dark Side of the Moon
#endif
#if GL_IMG_TBL_SIZE > 2
    #define IMG_FID_SGRM     2   // Shoyler and Glaude ans Rick and Morty
#endif
#if GL_IMG_TBL_SIZE > 3
    #define IMG_FID_WYWH     3   // Wish You Were Here
#endif
#if GL_IMG_TBL_SIZE > 4
    #define IMG_FID_CSH      4   // CSH
#endif
#if GL_IMG_TBL_SIZE > 5
    #define IMG_FID_INIT     5   // Initech
#endif
#if GL_IMG_TBL_SIZE > 6
    #define IMG_FID_HMMR     6   // The Wall hammers
#endif
#if GL_IMG_TBL_SIZE > 7
    #define IMG_FID_FCLS     7   // WYWH Faceless salesman
#endif
#if GL_IMG_TBL_SIZE > 8
    #define IMG_FID_APRT     8   // Aperture Science
#endif
#if GL_IMG_TBL_SIZE > 9
    #define IMG_FID_DEEP     9   // Deep 13
#endif
#if GL_IMG_TBL_SIZE > 10
    #define IMG_FID_JPLG    10   // Jurassic Park Logo
#endif
#if GL_IMG_TBL_SIZE > 11
    #define IMG_FID_JPIN    11   // Jurassic Park InGen Logo
#endif
#if GL_IMG_TBL_SIZE > 12
    #define IMG_FID_JPDA    12   // Jurassic Park DNA guy
#endif
#if GL_IMG_TBL_SIZE > 13
    #define IMG_FID_HAND    13   // WYWH Hands shaking
#endif

/** Globals    **/

/** Structures **/

/** Functions  **/

#endif
