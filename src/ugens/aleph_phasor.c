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
 * @file    aleph_phasor.c
 *
 * @brief   Phasors.
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_phasor.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

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

    ph->phase = ALEPH_PHASOR_DEFAULT_PHASE;
    ph->freq = ALEPH_PHASOR_DEFAULT_FREQ;
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

void Aleph_Phasor_next_block(Aleph_Phasor *const phasor, fract32 *buffer,
                             size_t size) {

    t_Aleph_Phasor *ph = *phasor;

    int i;
    for (i = 0; i < size; i++) {

        ph->phase += ph->freq;

        buffer[i] = ph->phase;
    }
}

void Aleph_Phasor_next_block_smooth(Aleph_Phasor *const phasor, fract32 *freq,
                                    fract32 *buffer, size_t size) {

    t_Aleph_Phasor *ph = *phasor;

    int i;
    for (i = 0; i < size; i++) {

        ph->freq = freq[i];

        ph->phase += ph->freq;

        buffer[i] = ph->phase;
    }
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

int32_t Aleph_Phasor_read(Aleph_Phasor *const phasor, fract32 freq) {

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

void Aleph_Quasor_init(Aleph_Quasor *quasor, t_Aleph *aleph) {

    Aleph_Quasor_init_to_pool(quasor, &aleph->mempool);
}

void Aleph_Quasor_init_to_pool(Aleph_Quasor *const quasor,
                               Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_Quasor *qu = *quasor =
        (t_Aleph_Quasor *)mpool_alloc(sizeof(t_Aleph_Quasor), mp);

    qu->mempool = mp;

    qu->sin_phase = ALEPH_QUASOR_DEFAULT_SIN_PHASE;
    qu->cos_phase = ALEPH_QUASOR_DEFAULT_COS_PHASE;
    qu->freq = ALEPH_QUASOR_DEFAULT_FREQ;
}

void Aleph_Quasor_free(Aleph_Quasor *const quasor) {

    t_Aleph_Quasor *qu = *quasor;

    mpool_free((char *)qu, qu->mempool);
}

void Aleph_Quasor_set_freq(Aleph_Quasor *const quasor, fract32 freq) {

    t_Aleph_Quasor *qu = *quasor;

    qu->freq = freq;
}

void Aleph_Quasor_set_cos_phase(Aleph_Quasor *const quasor, int32_t phase) {

    t_Aleph_Quasor *qu = *quasor;

    qu->cos_phase = phase;
    qu->sin_phase = qu->cos_phase + FR32_MAX;
}

void Aleph_Quasor_set_sin_phase(Aleph_Quasor *const quasor, int32_t phase) {

    t_Aleph_Quasor *qu = *quasor;

    qu->sin_phase = phase;
    qu->cos_phase = qu->sin_phase - FR32_MAX;
}

void Aleph_Quasor_advance(Aleph_Quasor *const quasor) {

    t_Aleph_Quasor *qu = *quasor;

    qu->cos_phase += qu->freq;
    qu->sin_phase = qu->cos_phase + FR32_MAX;
}

void Aleph_Quasor_advance_dynamic(Aleph_Quasor *const quasor, fract32 freq) {

    t_Aleph_Quasor *qu = *quasor;

    qu->cos_phase += freq;
    qu->sin_phase = qu->cos_phase + FR32_MAX;
}

int32_t Aleph_Quasor_sin_read(Aleph_Quasor *quasor) {

    t_Aleph_Quasor *qu = *quasor;

    return qu->sin_phase;
}

int32_t Aleph_Quasor_cos_read(Aleph_Quasor *quasor) {

    t_Aleph_Quasor *qu = *quasor;

    return qu->cos_phase;
}

int32_t Aleph_Quasor_pos_sin_read(Aleph_Quasor *quasor) {

    t_Aleph_Quasor *qu = *quasor;

    return (int32_t)(((uint32_t)qu->sin_phase) / (uint32_t)2);
}

int32_t Aleph_Quasor_pos_cos_read(Aleph_Quasor *quasor) {

    t_Aleph_Quasor *qu = *quasor;

    return (int32_t)(((uint32_t)qu->cos_phase) / (uint32_t)2);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
