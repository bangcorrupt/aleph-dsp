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

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "ugens/phasor.h"

/*----- Macros and Definitions ---------------------------------------*/

#define WAVEFORM_DEFAULT_FREQ (220 << 16)
#define WAVEFORM_DEFAULT_PHASE (0)

typedef enum {
    WAVEFORM_SHAPE_SINE,
    WAVEFORM_SHAPE_TRIANGLE,
    WAVEFORM_SHAPE_SAW,
    WAVEFORM_SHAPE_SQUARE,
} e_Waveform_shape;

typedef struct {
    Mempool mempool;
    Phasor phasor;
    uint8_t shape;
} t_Waveform;

typedef t_Waveform *Waveform;

typedef struct {
    Mempool mempool;
    Phasor phasor_a;
    Phasor phasor_b;
    uint8_t shape_a;
    uint8_t shape_b;
} t_WaveformDual;

typedef t_WaveformDual *WaveformDual;

typedef struct {
    Mempool mempool;
    Phasor phasor[ALEPH_MAX_UNISON_VOICES];
    uint8_t nvoices;
    uint8_t shape;
    fract32 spread;
} t_WaveformUnison;

typedef t_WaveformUnison *WaveformUnison;

/*----- Extern variable declarations ---------------------------------*/

void Waveform_init(Waveform *const wave, t_Aleph *aleph);
void Waveform_init_to_pool(Waveform *const wave, Mempool *const mempool);
void Waveform_free(Waveform *const wave);

fract32 Waveform_next(Waveform *const wave);
void Waveform_set_shape(Waveform *const wave, e_Waveform_shape shape);
void Waveform_set_freq(Waveform *const wave, fract32 freq);
void Waveform_set_phase(Waveform *const wave, int32_t phase);

void WaveformDual_init(WaveformDual *const wave, t_Aleph *aleph);
void WaveformDual_init_to_pool(WaveformDual *const wave,
                               Mempool *const mempool);

void WaveformDual_free(WaveformDual *const wave);

fract32 WaveformDual_next(WaveformDual *const wave);
void WaveformDual_set_shape(WaveformDual *const wave, e_Waveform_shape shape);
void WaveformDual_set_freq(WaveformDual *const wave, fract32 freq);
void WaveformDual_set_phase(WaveformDual *const wave, int32_t phase);

void WaveformDual_set_shape_a(WaveformDual *const wave, e_Waveform_shape shape);
void WaveformDual_set_shape_b(WaveformDual *const wave, e_Waveform_shape shape);
void WaveformDual_set_freq_a(WaveformDual *const wave, fract32 freq);
void WaveformDual_set_freq_b(WaveformDual *const wave, fract32 freq);
void WaveformDual_set_phase_a(WaveformDual *const wave, int32_t phase);
void WaveformDual_set_phase_b(WaveformDual *const wave, int32_t phase);

/*----- Extern function prototypes -----------------------------------*/

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
