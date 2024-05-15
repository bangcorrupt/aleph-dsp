/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by monome, modified by bangcorrupt 2024. */

/*
 * @file    aleph_window.c
 *
 * @brief   Window functions.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_utils.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

// This guy is a tophat with rounded edges
fract32 flat_top_env(fract32 pos, fract32 fade_ratio) {
    pos = max_fr1x32(pos, 0);
    if (pos <= fade_ratio)
        return FADE_IN(mult_fr1x32x32(FR32_MAX / fade_ratio, pos));
    else if (FR32_MAX - pos <= fade_ratio)
        return FADE_IN(mult_fr1x32x32(FR32_MAX / fade_ratio, FR32_MAX - pos));
    else
        return FR32_MAX;
}

fract32 half_wave_env(fract32 pos) {
    pos = max_fr1x32(pos, 0);
    if (pos <= FR32_MAX / 2)
        return FADE_IN(pos * 2);
    else if (FR32_MAX - pos <= FR32_MAX / 2)
        return FADE_IN((FR32_MAX - pos) * 2);
    else
        return FR32_MAX;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
