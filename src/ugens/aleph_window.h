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
 * @file    aleph_window.c
 *
 * @brief   Window functions.
 *
 */

/// TODO: Maybe fold this into another module, or move back to ricks_tricks.

#ifndef ALEPH_WINDOW_H
#define ALEPH_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

fract32 flat_top_env(int32_t pos, int32_t fade_ratio);
fract32 half_wave_env(fract32 pos);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
