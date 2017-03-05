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
// conditionnally added; not enough space for some images
#ifdef IMG_FID_HSC
    #include "../res/img_cxpm/hsc_logo.cxpm"
#endif
#ifdef IMG_FID_DSTM
    #include "../res/img_cxpm/dark_side_of_the_moon.cxpm"
#endif
#ifdef IMG_FID_WYWH
    #include "../res/img_cxpm/wish_you_were_here_small.cxpm"
#endif
#ifdef IMG_FID_SGRM
    #include "../res/img_cxpm/me_rick_and_morty.cxpm"
#endif
#ifdef IMG_FID_CSH
    #include "../res/img_cxpm/csh_logo.cxpm"
#endif
#ifdef IMG_FID_INIT
    #include "../res/img_cxpm/initech_logo.cxpm"
#endif
#ifdef IMG_FID_HMMR
    #include "../res/img_cxpm/hammers.cxpm"
#endif
#ifdef IMG_FID_FCLS
    #include "../res/img_cxpm/faceless_man.cxpm"
#endif
#ifdef IMG_FID_APRT
    #include "../res/img_cxpm/aperture_logo.cxpm"
#endif
#ifdef IMG_FID_DEEP
    #include "../res/img_cxpm/deep13_logo.cxpm"
#endif
#ifdef IMG_FID_JPLG
    #include "../res/img_cxpm/jp_logo.cxpm"
#endif
#ifdef IMG_FID_JPIN
    #include "../res/img_cxpm/jp_ingen.cxpm"
#endif
#ifdef IMG_FID_JPDA
    #include "../res/img_cxpm/jp_dna.cxpm"
#endif
#ifdef IMG_FID_HAND
    #include "../res/img_cxpm/wish_you_were_here_logo.cxpm"
#endif

/** Macros     **/

/** Globals    **/
static const uint8_t** gl_img_tbl[GL_IMG_TBL_SIZE] =
{
    #ifdef IMG_FID_HSC
        hsc_logo,
    #endif
    #ifdef IMG_FID_DSTM
        dark_side_of_the_moon,
    #endif
    #ifdef IMG_FID_WYWH
        wish_you_were_here_small,
    #endif
    #ifdef IMG_FID_SGRM
        me_rick_and_morty,
    #endif
    #ifdef IMG_FID_CSH
        csh_logo,
    #endif
    #ifdef IMG_FID_INIT
        initech_logo,
    #endif
    #ifdef IMG_FID_HMMR
        hammers,
    #endif
    #ifdef IMG_FID_FCLS
        faceless_man,
    #endif
    #ifdef IMG_FID_APRT
        aperture_logo,
    #endif
    #ifdef IMG_FID_DEEP
        deep13_logo,
    #endif
    #ifdef IMG_FID_JPLG
        jp_logo,
    #endif
    #ifdef IMG_FID_JPIN
        jp_ingen,
    #endif
    #ifdef IMG_FID_JPDA
        jp_dna,
    #endif
    #ifdef IMG_FID_HAND
        wish_you_were_here_logo,
    #endif
};

/** Structures **/

/** Functions  **/

#endif
