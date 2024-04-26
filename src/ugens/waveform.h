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

#include "aleph.h"

#include "ugens/phasor.h"

/*----- Macros and Definitions ---------------------------------------*/

#define WAVEFORM_DEFAULT_FREQ (220 << 16)

typedef struct {
    t_Phasor *phasor;
    uint8_t shape;
} t_Waveform;

typedef enum {
    WAVEFORM_SHAPE_SINE,
    WAVEFORM_SHAPE_TRIANGLE,
    WAVEFORM_SHAPE_SAW,
    WAVEFORM_SHAPE_SQUARE,
} e_Waveform_shape;

/*----- Extern variable declarations ---------------------------------*/

void Waveform_init(t_Waveform *wave, t_Aleph *aleph);
void Waveform_init_to_pool(t_Waveform *wave, t_Mempool *mp);
fract32 Waveform_next(t_Waveform *wave);
void Waveform_set_shape(t_Waveform *wave, uint8_t shape);
void Waveform_set_freq(t_Waveform *wave, fract32 freq);
void Waveform_set_phase(t_Waveform *wave, fract32 phase);

/*----- Extern function prototypes -----------------------------------*/

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
