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
 * @file    waveform.c
 *
 * @brief   Anti-aliased basic waveform oscillators.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include <stdint.h>

#include "fract_math.h"

#include "aleph-mempool.h"
#include "aleph.h"

#include "ugens/osc_polyblep.h"
#include "ugens/phasor.h"
#include "ugens/waveform.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Waveform_init(Waveform *const wave, t_Aleph *const aleph) {
    //
    Waveform_init_to_pool(wave, &aleph->mempool);
}

void Waveform_init_to_pool(Waveform *const wave, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Waveform *wv = *wave = (t_Waveform *)mpool_alloc(sizeof(t_Waveform), mp);

    wv->mempool = mp;

    wv->shape = WAVEFORM_SHAPE_SINE;

    // Phasor_init(&wv->phasor, mp->leaf);
    Phasor_init_to_pool(&wv->phasor, mempool);
    Phasor_set_freq(&wv->phasor, WAVEFORM_DEFAULT_FREQ);
}

void Waveform_free(Waveform *const wave) {

    t_Waveform *wv = *wave;

    Phasor_free(&wv->phasor);

    mpool_free((char *)wv, wv->mempool);
}

fract32 Waveform_next(Waveform *const wave) {

    t_Waveform *wv = *wave;

    fract32 next;

    Phasor_next(&wv->phasor);

    switch (wv->shape) {

    case WAVEFORM_SHAPE_SINE:
        next = sine_polyblep(wv->phasor->phase);
        break;

    case WAVEFORM_SHAPE_TRIANGLE:
        next = triangle_polyblep(wv->phasor->phase);
        break;

    case WAVEFORM_SHAPE_SAW:
        next = saw_polyblep(wv->phasor->phase, wv->phasor->freq);
        break;

    case WAVEFORM_SHAPE_SQUARE:
        next = square_polyblep(wv->phasor->phase, wv->phasor->freq);
        break;

    default:
        next = 0;
        break;
    }

    return shl_fr1x32(next, 16);
}

void Waveform_set_shape(Waveform *const wave, uint8_t shape) {
    //
    t_Waveform *wv = *wave;

    wv->shape = shape;
}

void Waveform_set_freq(Waveform *const wave, fract32 freq) {
    //
    t_Waveform *wv = *wave;

    Phasor_set_freq(&wv->phasor, freq);
}

void Waveform_set_phase(Waveform *const wave, int32_t phase) {
    //
    t_Waveform *wv = *wave;

    Phasor_set_phase(&wv->phasor, phase);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
