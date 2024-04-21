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
    s32 phase;
    fract32 freq;
} phasor;

typedef struct {
    s32 cosPhase;
    s32 sinPhase;
} quadraturePhasor;

/*----- Extern variable declarations ---------------------------------*/

void phasor_init(phasor *phasor);
s32 phasor_next_dynamic(phasor *phasor, fract32 freq);
s32 phasor_next(phasor *phasor);
s32 phasor_read(phasor *phasor, s32 freq);
s32 phasor_pos_next_dynamic(phasor *phasor, fract32 freq);
s32 phasor_pos_read(phasor *phasor);

void quadraturePhasor_init(quadraturePhasor *phasor);
void quadraturePhasor_pos_next_dynamic(quadraturePhasor *phasor, fract32 freq);
s32 quadraturePhasor_pos_sinRead(quadraturePhasor *phasor);
s32 quadraturePhasor_pos_cosRead(quadraturePhasor *phasor);
s32 quadraturePhasor_sinRead(quadraturePhasor *phasor);
s32 quadraturePhasor_cosRead(quadraturePhasor *phasor);

/*----- Extern function prototypes -----------------------------------*/

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
