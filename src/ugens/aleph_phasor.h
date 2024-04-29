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
 * @file    aleph_phasor.h
 *
 * @brief   Public API for phasors.
 *
 */

#ifndef ALEPH_PHASOR_H
#define ALEPH_PHASOR_H

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
} t_Aleph_Phasor;

typedef t_Aleph_Phasor *Aleph_Phasor;

typedef struct {
    int32_t cos_phase;
    int32_t sin_phase;
} t_Aleph_QuadraturePhasor;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

/// TODO: Functions to set frequency.
///          Aleph_QuadraturePhasor_next()
//
void Aleph_Phasor_init(Aleph_Phasor *const phasor, t_Aleph *const aleph);
void Aleph_Phasor_init_to_pool(Aleph_Phasor *const phasor,
                               Mempool *const mempool);
void Aleph_Phasor_free(Aleph_Phasor *const phasor);
int32_t Aleph_Phasor_next(Aleph_Phasor *const phasor);
void Aleph_Phasor_set_freq(Aleph_Phasor *const phasor, fract32 freq);
void Aleph_Phasor_set_phase(Aleph_Phasor *const phasor, int32_t phase);
int32_t Aleph_Phasor_next_dynamic(Aleph_Phasor *const phasor, fract32 freq);
int32_t Aleph_Phasor_read(Aleph_Phasor *const phasor, int32_t freq);
int32_t Aleph_Phasor_pos_next_dynamic(Aleph_Phasor *const phasor, fract32 freq);
int32_t Aleph_Phasor_pos_read(Aleph_Phasor *const phasor);

void Aleph_QuadraturePhasor_init(t_Aleph_QuadraturePhasor *phasor);
void Aleph_QuadraturePhasor_pos_next_dynamic(t_Aleph_QuadraturePhasor *phasor,
                                             fract32 freq);
int32_t Aleph_QuadraturePhasor_pos_sin_read(t_Aleph_QuadraturePhasor *phasor);
int32_t Aleph_QuadraturePhasor_pos_cos_read(t_Aleph_QuadraturePhasor *phasor);
int32_t Aleph_QuadraturePhasor_sin_read(t_Aleph_QuadraturePhasor *phasor);
int32_t Aleph_QuadraturePhasor_cos_read(t_Aleph_QuadraturePhasor *phasor);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
