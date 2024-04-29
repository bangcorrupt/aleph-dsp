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
 * @file    aleph_soft_clip.h
 *
 * @brief   Public API for soft clipping.
 *
 */

#ifndef ALEPH_SOFT_CLIP_H
#define ALEPH_SOFT_CLIP_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

fract32 soft_clip(fract32 lim, fract32 in);
fract32 soft_clip_norm(fract32 lim, fract32 in);
fract32 soft_clip_asym(fract32 lim_pos, fract32 lim_neg, fract32 in);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
