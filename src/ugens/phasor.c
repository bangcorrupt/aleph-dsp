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

void Phasor_init(t_Phasor *phasor) { phasor->phase = 0; }

int32_t Phasor_next_dynamic(t_Phasor *phasor, fract32 freq) {
    phasor->phase += freq;
    return phasor->phase;
}

int32_t Phasor_next(t_Phasor *phasor) {
    phasor->phase += phasor->freq;
    return phasor->phase;
}

int32_t Phasor_read(t_Phasor *phasor, int32_t freq) { return phasor->phase; }

int32_t Phasor_pos_next_dynamic(t_Phasor *phasor, fract32 freq) {
    phasor->phase += freq;
    return ((u32)phasor->phase) / 2;
}

int32_t Phasor_pos_read(t_Phasor *phasor) { return ((u32)phasor->phase) / 2; }

void QuadraturePhasor_init(t_QuadraturePhasor *phasor) {
    phasor->sinPhase = 0;
    phasor->cosPhase = FR32_MAX / 2;
}

void QuadraturePhasor_pos_next_dynamic(t_QuadraturePhasor *phasor,
                                       fract32 freq) {
    phasor->cosPhase += freq;
    phasor->sinPhase = phasor->cosPhase + FR32_MAX;
}

int32_t QuadraturePhasor_sinRead(t_QuadraturePhasor *phasor) {
    return phasor->sinPhase;
}

int32_t QuadraturePhasor_cosRead(t_QuadraturePhasor *phasor) {
    return phasor->cosPhase;
}

int32_t QuadraturePhasor_pos_sinRead(t_QuadraturePhasor *phasor) {
    return (int32_t)(((u32)phasor->sinPhase) / (u32)2);
}

int32_t QuadraturePhasor_pos_cosRead(t_QuadraturePhasor *phasor) {
    return (int32_t)(((u32)phasor->cosPhase) / (u32)2);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
