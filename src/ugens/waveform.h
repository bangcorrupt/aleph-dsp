/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/Aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/*
 * @file    waveform.h
 *
 * @brief   Public ASPI for anti-aliased basic waveform oscillators.
 *
 */

#ifndef ALEPH_WAVEFORM_H
#define ALEPH_WAVEFORM_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/
#include <stdint.h>

#include "aleph.h"

#include "ugens/phasor.h"

/*----- Macros and Definitions ---------------------------------------*/

#define WAVEFORM_DEFAULT_FREQ (220 << 16)

typedef struct {
    t_Mempool *mempool;
    t_Phasor *phasor;
    uint8_t shape;
} t_Waveform;

typedef t_Waveform *Waveform;

typedef enum {
    WAVEFORM_SHAPE_SINE,
    WAVEFORM_SHAPE_TRIANGLE,
    WAVEFORM_SHAPE_SAW,
    WAVEFORM_SHAPE_SQUARE,
} e_Waveform_shape;

/*----- Extern variable declarations ---------------------------------*/

void Waveform_init(Waveform *const wave, t_Aleph *aleph);
void Waveform_init_to_pool(Waveform *const wave, t_Mempool **mp);
fract32 Waveform_next(Waveform *const wave);
void Waveform_set_shape(Waveform *const wave, uint8_t shape);
void Waveform_set_freq(Waveform *const wave, fract32 freq);
void Waveform_set_phase(Waveform *const wave, int32_t phase);

/*----- Extern function prototypes -----------------------------------*/

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
