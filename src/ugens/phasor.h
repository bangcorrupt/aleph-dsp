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

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef struct {
    int32_t phase;
    fract32 freq;
} t_Phasor;

typedef struct {
    int32_t cos_phase;
    int32_t sin_phase;
} t_QuadraturePhasor;

/*----- Extern variable declarations ---------------------------------*/

/// TODO: Functions to set frequency.
///         QuadraturePhasor_next()
//
void Phasor_init(t_Phasor *phasor);
int32_t Phasor_next_dynamic(t_Phasor *phasor, fract32 freq);
int32_t Phasor_next(t_Phasor *phasor);
int32_t Phasor_read(t_Phasor *phasor, int32_t freq);
int32_t Phasor_pos_next_dynamic(t_Phasor *phasor, fract32 freq);
int32_t Phasor_pos_read(t_Phasor *phasor);

void QuadraturePhasor_init(t_QuadraturePhasor *phasor);
void QuadraturePhasor_pos_next_dynamic(t_QuadraturePhasor *phasor,
                                       fract32 freq);
int32_t QuadraturePhasor_pos_sin_read(t_QuadraturePhasor *phasor);
int32_t QuadraturePhasor_pos_cos_read(t_QuadraturePhasor *phasor);
int32_t QuadraturePhasor_sin_read(t_QuadraturePhasor *phasor);
int32_t QuadraturePhasor_cos_read(t_QuadraturePhasor *phasor);

/*----- Extern function prototypes -----------------------------------*/

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
