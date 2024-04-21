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

#ifndef ALEPH_WINDOW_H
#define ALEPH_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "types.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

fract32 s32_flatTop_env(s32 pos, s32 fadeRatio);
fract32 s32_halfWave_env(fract32 pos);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
