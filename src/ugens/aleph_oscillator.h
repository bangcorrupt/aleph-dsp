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
 * @file    aleph_oscillator.h
 *
 * @brief   Public API for oscillators.
 */

#ifndef ALEPH_OSCILLATOR_H
#define ALEPH_OSCILLATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_phasor.h"

/*----- Macros -------------------------------------------------------*/

#define ALEPH_OSCILLATOR_DEFAULT_FREQ (1)
#define ALEPH_OSCILLATOR_DEFAULT_PHASE (0)

/// TODO:  Make saw bipolar.
//

/*----- Typedefs -----------------------------------------------------*/

typedef enum {
    ALEPH_OSCILLATOR_SHAPE_SINE,
    ALEPH_OSCILLATOR_SHAPE_TRIANGLE,
    ALEPH_OSCILLATOR_SHAPE_SAW,
    ALEPH_OSCILLATOR_SHAPE_SQUARE,
} e_Aleph_Oscillator_shape;

typedef struct {
    Mempool mempool;
    Aleph_Phasor phasor;
    e_Aleph_Oscillator_shape shape;
} t_Aleph_Oscillator;

typedef t_Aleph_Oscillator *Aleph_Oscillator;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_Oscillator_init(Aleph_Oscillator *const oscillator,
                           t_Aleph *const aleph);
void Aleph_Oscillator_init_to_pool(Aleph_Oscillator *const oscillator,
                                   Mempool *const mempool);

void Aleph_Oscillator_free(Aleph_Oscillator *const oscillator);

void Aleph_Oscillator_set_freq(Aleph_Oscillator *const oscillator,
                               fract32 freq);
void Aleph_Oscillator_set_phase(Aleph_Oscillator *const oscillator,
                                fract32 phase);
void Aleph_Oscillator_set_shape(Aleph_Oscillator *const oscillator,
                                e_Aleph_Oscillator_shape shape);

fract32 Aleph_Oscillator_next(Aleph_Oscillator *const oscillator);
fract16 Aleph_Oscillator_16_next(Aleph_Oscillator *const oscillator);

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
