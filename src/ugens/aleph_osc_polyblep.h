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

/**
 * @file    aleph_osc_polyblep.h
 *
 * @brief   Public API for oscillator polyblep.
 */

#ifndef ALEPH_OSC_POLYBLEP_H
#define ALEPH_OSC_POLYBLEP_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

fract16 saw_polyblep(fract32 p, fract32 dp);
fract16 square_polyblep(fract32 p, fract32 dp);
fract16 sine_polyblep(fract32 phase);
fract16 triangle_polyblep(fract32 phase);

void saw_polyblep_block(fract32 *phase, fract32 freq, fract32 *output,
                        size_t size);

void saw_polyblep_block_smooth(fract32 *phase, fract32 *freq, fract32 *output,
                               size_t size);

void square_polyblep_block(fract32 *phase, fract32 freq, fract32 *output,
                           size_t size);

void square_polyblep_block_smooth(fract32 *phase, fract32 *freq,
                                  fract32 *output, size_t size);

void sine_polyblep_block(fract32 *phase, fract32 *output, size_t size);

void triangle_polyblep_block(fract32 *phase, fract32 *output, size_t size);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
