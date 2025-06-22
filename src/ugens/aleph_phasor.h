/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/**
 * @file    aleph_phasor.h
 *
 * @brief   Public API for phasors.
 */

#ifndef ALEPH_PHASOR_H
#define ALEPH_PHASOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros -------------------------------------------------------*/

#define ALEPH_PHASOR_DEFAULT_PHASE (0)
#define ALEPH_PHASOR_DEFAULT_FREQ (1)

#define ALEPH_QUASOR_DEFAULT_SIN_PHASE (0)
#define ALEPH_QUASOR_DEFAULT_COS_PHASE (FR32_MAX / 2)
#define ALEPH_QUASOR_DEFAULT_FREQ (1)

/*----- Typedefs -----------------------------------------------------*/

typedef struct {
    t_Mempool *mempool;
    int32_t phase;
    fract32 freq;
} t_Aleph_Phasor;

typedef t_Aleph_Phasor *Aleph_Phasor;

typedef struct {
    t_Mempool *mempool;
    int32_t cos_phase;
    int32_t sin_phase;
    fract32 freq;
} t_Aleph_Quasor;

typedef t_Aleph_Quasor *Aleph_Quasor;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

/// TODO: Functions to set frequency.
///          Aleph_Quasor_next()
//
void Aleph_Phasor_init(Aleph_Phasor *const phasor, t_Aleph *const aleph);
void Aleph_Phasor_init_to_pool(Aleph_Phasor *const phasor,
                               Mempool *const mempool);
void Aleph_Phasor_free(Aleph_Phasor *const phasor);
int32_t Aleph_Phasor_next(Aleph_Phasor *const phasor);
void Aleph_Phasor_set_freq(Aleph_Phasor *const phasor, fract32 freq);
void Aleph_Phasor_set_phase(Aleph_Phasor *const phasor, int32_t phase);
int32_t Aleph_Phasor_next_dynamic(Aleph_Phasor *const phasor, fract32 freq);
int32_t Aleph_Phasor_read(Aleph_Phasor *const phasor, fract32 freq);
int32_t Aleph_Phasor_pos_next_dynamic(Aleph_Phasor *const phasor, fract32 freq);
int32_t Aleph_Phasor_pos_read(Aleph_Phasor *const phasor);

void Aleph_Quasor_init(Aleph_Quasor *quasor, t_Aleph *const aleph);
void Aleph_Quasor_init_to_pool(Aleph_Quasor *const quasor,
                               Mempool *const mempool);
void Aleph_Quasor_free(Aleph_Quasor *const quasor);
void Aleph_Quasor_advance(Aleph_Quasor *const quasor);
void Aleph_Quasor_advance_dynamic(Aleph_Quasor *quasor, fract32 freq);
void Aleph_Quasor_set_freq(Aleph_Quasor *const quasor, fract32 freq);
void Aleph_Quasor_set_cos_phase(Aleph_Quasor *const quasor, int32_t phase);
void Aleph_Quasor_set_sin_phase(Aleph_Quasor *const quasor, int32_t phase);
int32_t Aleph_Quasor_pos_sin_read(Aleph_Quasor *quasor);
int32_t Aleph_Quasor_pos_cos_read(Aleph_Quasor *quasor);
int32_t Aleph_Quasor_sin_read(Aleph_Quasor *quasor);
int32_t Aleph_Quasor_cos_read(Aleph_Quasor *quasor);

void Aleph_Phasor_next_block(Aleph_Phasor *const phasor, fract32 *output,
                             size_t size);

void Aleph_Phasor_next_block_smooth(Aleph_Phasor *const phasor, fract32 *freq,
                                    fract32 *buffer, size_t size);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
