/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/*
 * @file    aleph_waveform.h
 *
 * @brief   Public API for basic waveform oscillators using PolyBLEP.
 *
 */

#ifndef ALEPH_WAVEFORM_H
#define ALEPH_WAVEFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_phasor.h"

/*----- Macros -------------------------------------------------------*/

#define WAVEFORM_DEFAULT_FREQ (220 << 16)
#define WAVEFORM_DEFAULT_PHASE (0)


/*----- Typedefs -----------------------------------------------------*/

typedef enum {
    WAVEFORM_SHAPE_SINE,
    WAVEFORM_SHAPE_TRIANGLE,
    WAVEFORM_SHAPE_SAW,
    WAVEFORM_SHAPE_SQUARE,
} e_Aleph_Waveform_shape;

typedef struct {
    Mempool mempool;
    Aleph_Phasor phasor;
    uint8_t shape;
} t_Aleph_Waveform;

typedef t_Aleph_Waveform *Aleph_Waveform;

typedef struct {
    Mempool mempool;
    Aleph_Phasor phasor_a;
    Aleph_Phasor phasor_b;
    uint8_t shape_a;
    uint8_t shape_b;
} t_Aleph_WaveformDual;

typedef t_Aleph_WaveformDual *Aleph_WaveformDual;

typedef struct {
    Mempool mempool;
    Aleph_Phasor phasor[ALEPH_MAX_UNISON_VOICES];
    uint8_t nvoices;
    uint8_t shape;
    fract32 spread;
} t_Aleph_WaveformUnison;

typedef t_Aleph_WaveformUnison *Aleph_WaveformUnison;

/*----- Extern variable declarations ---------------------------------*/

void Aleph_Waveform_init(Aleph_Waveform *const wave, t_Aleph *aleph);
void Aleph_Waveform_init_to_pool(Aleph_Waveform *const wave,
                                 Mempool *const mempool);
void Aleph_Waveform_free(Aleph_Waveform *const wave);

fract32 Aleph_Waveform_next(Aleph_Waveform *const wave);
void Aleph_Waveform_set_shape(Aleph_Waveform *const wave,
                              e_Aleph_Waveform_shape shape);
void Aleph_Waveform_set_freq(Aleph_Waveform *const wave, fract32 freq);
void Aleph_Waveform_set_phase(Aleph_Waveform *const wave, int32_t phase);

void Aleph_WaveformDual_init(Aleph_WaveformDual *const wave, t_Aleph *aleph);
void Aleph_WaveformDual_init_to_pool(Aleph_WaveformDual *const wave,
                                     Mempool *const mempool);

void Aleph_WaveformDual_free(Aleph_WaveformDual *const wave);

fract32 Aleph_WaveformDual_next(Aleph_WaveformDual *const wave);
void Aleph_WaveformDual_set_shape(Aleph_WaveformDual *const wave,
                                  e_Aleph_Waveform_shape shape);
void Aleph_WaveformDual_set_freq(Aleph_WaveformDual *const wave, fract32 freq);
void Aleph_WaveformDual_set_phase(Aleph_WaveformDual *const wave,
                                  int32_t phase);

void Aleph_WaveformDual_set_shape_a(Aleph_WaveformDual *const wave,
                                    e_Aleph_Waveform_shape shape);
void Aleph_WaveformDual_set_shape_b(Aleph_WaveformDual *const wave,
                                    e_Aleph_Waveform_shape shape);
void Aleph_WaveformDual_set_freq_a(Aleph_WaveformDual *const wave,
                                   fract32 freq);
void Aleph_WaveformDual_set_freq_b(Aleph_WaveformDual *const wave,
                                   fract32 freq);
void Aleph_WaveformDual_set_phase_a(Aleph_WaveformDual *const wave,
                                    int32_t phase);
void Aleph_WaveformDual_set_phase_b(Aleph_WaveformDual *const wave,
                                    int32_t phase);

/*----- Extern function prototypes -----------------------------------*/

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
