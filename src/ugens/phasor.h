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
 * @file    phasor.h
 *
 * @brief   Public API for phasors.
 *
 */

#ifndef ALEPH_PHASOR_H
#define ALEPH_PHASOR_H

#include "aleph-mempool.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef struct {
    t_Mempool *mempool;
    int32_t phase;
    fract32 freq;
} t_Phasor;

typedef t_Phasor *Phasor;

typedef struct {
    int32_t cos_phase;
    int32_t sin_phase;
} t_QuadraturePhasor;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

/// TODO: Functions to set frequency.
///         QuadraturePhasor_next()
//
void Phasor_init(Phasor *const phasor, t_Aleph *const aleph);
void Phasor_init_to_pool(Phasor *const phasor, Mempool *const mempool);
void Phasor_free(Phasor *const phasor);
int32_t Phasor_next(Phasor *const phasor);
void Phasor_set_freq(Phasor *const phasor, fract32 freq);
void Phasor_set_phase(Phasor *const phasor, int32_t phase);
int32_t Phasor_next_dynamic(Phasor *const phasor, fract32 freq);
int32_t Phasor_read(Phasor *const phasor, int32_t freq);
int32_t Phasor_pos_next_dynamic(Phasor *const phasor, fract32 freq);
int32_t Phasor_pos_read(Phasor *const phasor);

void QuadraturePhasor_init(t_QuadraturePhasor *phasor);
void QuadraturePhasor_pos_next_dynamic(t_QuadraturePhasor *phasor,
                                       fract32 freq);
int32_t QuadraturePhasor_pos_sin_read(t_QuadraturePhasor *phasor);
int32_t QuadraturePhasor_pos_cos_read(t_QuadraturePhasor *phasor);
int32_t QuadraturePhasor_sin_read(t_QuadraturePhasor *phasor);
int32_t QuadraturePhasor_cos_read(t_QuadraturePhasor *phasor);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
