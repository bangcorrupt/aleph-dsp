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
 * @file    aleph_phasor.c
 *
 * @brief   Phasors.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_phasor.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_Phasor_init(Aleph_Phasor *const phasor, t_Aleph *aleph) {

    Aleph_Phasor_init_to_pool(phasor, &aleph->mempool);
}

void Aleph_Phasor_init_to_pool(Aleph_Phasor *const phasor,
                               Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_Phasor *ph = *phasor =
        (t_Aleph_Phasor *)mpool_alloc(sizeof(t_Aleph_Phasor), mp);

    ph->mempool = mp;

    ph->phase = 0;
    ph->freq = 1;
}

void Aleph_Phasor_free(Aleph_Phasor *const phasor) {

    t_Aleph_Phasor *ph = *phasor;

    mpool_free((char *)ph, ph->mempool);
}

int32_t Aleph_Phasor_next(Aleph_Phasor *const phasor) {

    t_Aleph_Phasor *ph = *phasor;

    ph->phase += ph->freq;

    return ph->phase;
}

void Aleph_Phasor_set_freq(Aleph_Phasor *const phasor, fract32 freq) {

    t_Aleph_Phasor *ph = *phasor;

    ph->freq = freq;
}

void Aleph_Phasor_set_phase(Aleph_Phasor *const phasor, int32_t phase) {

    t_Aleph_Phasor *ph = *phasor;

    ph->phase = phase;
}

int32_t Aleph_Phasor_next_dynamic(Aleph_Phasor *const phasor, fract32 freq) {

    t_Aleph_Phasor *ph = *phasor;

    ph->phase += freq;

    return ph->phase;
}

int32_t Aleph_Phasor_read(Aleph_Phasor *const phasor, int32_t freq) {

    t_Aleph_Phasor *ph = *phasor;

    return ph->phase;
}

int32_t Aleph_Phasor_pos_next_dynamic(Aleph_Phasor *const phasor,
                                      fract32 freq) {

    t_Aleph_Phasor *ph = *phasor;

    ph->phase += freq;

    return ((uint32_t)ph->phase) / 2;
}

int32_t Aleph_Phasor_pos_read(Aleph_Phasor *const phasor) {

    t_Aleph_Phasor *ph = *phasor;

    return ((uint32_t)ph->phase) / 2;
}

void Aleph_QuadraturePhasor_init(t_Aleph_QuadraturePhasor *phasor) {

    phasor->sin_phase = 0;
    phasor->cos_phase = FR32_MAX / 2;
}

void Aleph_QuadraturePhasor_pos_next_dynamic(t_Aleph_QuadraturePhasor *phasor,
                                             fract32 freq) {
    phasor->cos_phase += freq;
    phasor->sin_phase = phasor->cos_phase + FR32_MAX;
}

int32_t Aleph_QuadraturePhasor_sin_read(t_Aleph_QuadraturePhasor *phasor) {

    return phasor->sin_phase;
}

int32_t Aleph_QuadraturePhasor_cos_read(t_Aleph_QuadraturePhasor *phasor) {

    return phasor->cos_phase;
}

int32_t Aleph_QuadraturePhasor_pos_sin_read(t_Aleph_QuadraturePhasor *phasor) {

    return (int32_t)(((uint32_t)phasor->sin_phase) / (uint32_t)2);
}

int32_t Aleph_QuadraturePhasor_pos_cos_read(t_Aleph_QuadraturePhasor *phasor) {

    return (int32_t)(((uint32_t)phasor->cos_phase) / (uint32_t)2);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
