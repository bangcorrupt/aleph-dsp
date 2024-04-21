/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/Aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by monome, modified by bangcorrupt 2024. */

/*
 * @file    window.c
 *
 * @brief   Window functions.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "fix.h"
#include "fract_math.h"
#include "types.h"

#include "ricks_tricks.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

// This guy is a tophat with rounded edges
fract32 s32_flatTop_env(fract32 pos, fract32 fadeRatio) {
    pos = max_fr1x32(pos, 0);
    if (pos <= fadeRatio)
        return fadeIn(mult_fr1x32x32(FR32_MAX / fadeRatio, pos));
    else if (FR32_MAX - pos <= fadeRatio)
        return fadeIn(mult_fr1x32x32(FR32_MAX / fadeRatio, FR32_MAX - pos));
    else
        return FR32_MAX;
}

fract32 s32_halfWave_env(fract32 pos) {
    pos = max_fr1x32(pos, 0);
    if (pos <= FR32_MAX / 2)
        return fadeIn(pos * 2);
    else if (FR32_MAX - pos <= FR32_MAX / 2)
        return fadeIn((FR32_MAX - pos) * 2);
    else
        return FR32_MAX;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
