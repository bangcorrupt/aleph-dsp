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
 * @file    phasor.c
 *
 * @brief   Phasors.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "phasor.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void phasor_init(phasor *phasor) { phasor->phase = 0; }

s32 phasor_next_dynamic(phasor *phasor, fract32 freq) {
    phasor->phase += freq;
    return phasor->phase;
}

s32 phasor_next(phasor *phasor) {
    phasor->phase += phasor->freq;
    return phasor->phase;
}

s32 phasor_read(phasor *phasor, s32 freq) { return phasor->phase; }

s32 phasor_pos_next_dynamic(phasor *phasor, fract32 freq) {
    phasor->phase += freq;
    return ((u32)phasor->phase) / 2;
}

s32 phasor_pos_read(phasor *phasor) { return ((u32)phasor->phase) / 2; }

void quadraturePhasor_init(quadraturePhasor *phasor) {
    phasor->sinPhase = 0;
    phasor->cosPhase = FR32_MAX / 2;
}

void quadraturePhasor_pos_next_dynamic(quadraturePhasor *phasor, fract32 freq) {
    phasor->cosPhase += freq;
    phasor->sinPhase = phasor->cosPhase + FR32_MAX;
}

s32 quadraturePhasor_sinRead(quadraturePhasor *phasor) {
    return phasor->sinPhase;
}

s32 quadraturePhasor_cosRead(quadraturePhasor *phasor) {
    return phasor->cosPhase;
}

s32 quadraturePhasor_pos_sinRead(quadraturePhasor *phasor) {
    return (s32)(((u32)phasor->sinPhase) / (u32)2);
}

s32 quadraturePhasor_pos_cosRead(quadraturePhasor *phasor) {
    return (s32)(((u32)phasor->cosPhase) / (u32)2);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
