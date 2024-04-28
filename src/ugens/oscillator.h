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

#include "aleph-mempool.h"
#include "aleph.h"

#include "ugens/phasor.h"

/*----- Macros and Definitions ---------------------------------------*/

#define OSCILLATOR_DEFAULT_FREQ (1)
#define OSCILLATOR_DEFAULT_PHASE (0)

/// TODO:  Make saw bipolar.
//
typedef enum {
    OSCILLATOR_SHAPE_SINE,
    OSCILLATOR_SHAPE_TRIANGLE,
    /* OSCILLATOR_SHAPE_SAW, */
    OSCILLATOR_SHAPE_SQUARE,
} e_Oscillator_shape;

typedef struct {
    Mempool mempool;
    Phasor phasor;
    e_Oscillator_shape shape;
} t_Oscillator;

typedef t_Oscillator *Oscillator;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Oscillator_init(Oscillator *const oscillator, t_Aleph *const aleph);
void Oscillator_init_to_pool(Oscillator *const oscillator,
                             Mempool *const mempool);

void Oscillator_set_freq(Oscillator *const oscillator, fract32 freq);
void Oscillator_set_phase(Oscillator *const oscillator, fract32 phase);
void Oscillator_set_shape(Oscillator *const oscillator,
                          e_Oscillator_shape shape);

fract32 Oscillator_next(Oscillator *const oscillator);
fract16 Oscillator_16_next(Oscillator *const oscillator);

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
