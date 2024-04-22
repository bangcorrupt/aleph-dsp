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
 * @file    oscillator.h
 *
 * @brief   Public API for oscillators.
 *
 */

#ifndef ALEPH_OSCILLATOR_H
#define ALEPH_OSCILLATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "types.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

fract32 osc_sin(fract32 phase);
fract16 osc_sin16(fract32 phase);
fract32 osc_triangle(fract32 phase);
fract16 osc_triangle16(fract32 phase);
fract32 osc_square(fract32 phase);
fract16 osc_square16(fract32 phase);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
