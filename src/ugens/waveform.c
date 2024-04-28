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
    Phasor_set_phase(&wv->phasor, WAVEFORM_DEFAULT_PHASE);
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

void Waveform_set_shape(Waveform *const wave, e_Waveform_shape shape) {

    t_Waveform *wv = *wave;

    wv->shape = shape;
}

void Waveform_set_freq(Waveform *const wave, fract32 freq) {

    t_Waveform *wv = *wave;

    Phasor_set_freq(&wv->phasor, freq);
}

void Waveform_set_phase(Waveform *const wave, int32_t phase) {

    t_Waveform *wv = *wave;

    Phasor_set_phase(&wv->phasor, phase);
}

void WaveformDual_init(WaveformDual *const wave, t_Aleph *const aleph) {

    WaveformDual_init_to_pool(wave, &aleph->mempool);
}

void WaveformDual_init_to_pool(WaveformDual *const wave,
                               Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_WaveformDual *wv = *wave =
        (t_WaveformDual *)mpool_alloc(sizeof(t_WaveformDual), mp);

    wv->mempool = mp;

    wv->shape_a = WAVEFORM_SHAPE_SINE;
    wv->shape_b = WAVEFORM_SHAPE_SINE;

    Phasor_init_to_pool(&wv->phasor_a, mempool);
    Phasor_set_freq(&wv->phasor_a, WAVEFORM_DEFAULT_FREQ);
    Phasor_set_phase(&wv->phasor_a, WAVEFORM_DEFAULT_PHASE);

    Phasor_init_to_pool(&wv->phasor_b, mempool);
    Phasor_set_freq(&wv->phasor_b, WAVEFORM_DEFAULT_FREQ);
    Phasor_set_phase(&wv->phasor_b, WAVEFORM_DEFAULT_PHASE);
}

void WaveformDual_free(WaveformDual *const wave) {

    t_WaveformDual *wv = *wave;

    Phasor_free(&wv->phasor_a);
    Phasor_free(&wv->phasor_b);

    mpool_free((char *)wv, wv->mempool);
}

fract32 WaveformDual_next(WaveformDual *const wave) {

    t_WaveformDual *wv = *wave;

    fract32 next_a;
    fract32 next_b;

    Phasor_next(&wv->phasor_a);
    Phasor_next(&wv->phasor_b);

    switch (wv->shape_a) {

    case WAVEFORM_SHAPE_SINE:
        next_a = sine_polyblep(wv->phasor_a->phase);
        break;

    case WAVEFORM_SHAPE_TRIANGLE:
        next_a = triangle_polyblep(wv->phasor_a->phase);
        break;

    case WAVEFORM_SHAPE_SAW:
        next_a = saw_polyblep(wv->phasor_a->phase, wv->phasor_a->freq);
        break;

    case WAVEFORM_SHAPE_SQUARE:
        next_a = square_polyblep(wv->phasor_a->phase, wv->phasor_a->freq);
        break;

    default:
        next_a = 0;
        break;
    }

    switch (wv->shape_b) {

    case WAVEFORM_SHAPE_SINE:
        next_b = sine_polyblep(wv->phasor_b->phase);
        break;

    case WAVEFORM_SHAPE_TRIANGLE:
        next_b = triangle_polyblep(wv->phasor_b->phase);
        break;

    case WAVEFORM_SHAPE_SAW:
        next_b = saw_polyblep(wv->phasor_b->phase, wv->phasor_b->freq);
        break;

    case WAVEFORM_SHAPE_SQUARE:
        next_b = square_polyblep(wv->phasor_b->phase, wv->phasor_b->freq);
        break;

    default:
        next_b = 0;
        break;
    }

    return add_fr1x32(shl_fr1x32(next_a, 15), shl_fr1x32(next_b, 15));
}

void WaveformDual_set_shape(WaveformDual *const wave, e_Waveform_shape shape) {

    t_WaveformDual *wv = *wave;

    wv->shape_a = shape;
    wv->shape_b = shape;
}

void WaveformDual_set_freq(WaveformDual *const wave, fract32 freq) {

    t_WaveformDual *wv = *wave;

    Phasor_set_freq(&wv->phasor_a, freq);
    Phasor_set_freq(&wv->phasor_b, freq);
}

void WaveformDual_set_phase(WaveformDual *const wave, int32_t phase) {

    t_WaveformDual *wv = *wave;

    Phasor_set_phase(&wv->phasor_a, phase);
    Phasor_set_phase(&wv->phasor_b, phase);
}

void WaveformDual_set_shape_a(WaveformDual *const wave,
                              e_Waveform_shape shape) {

    t_WaveformDual *wv = *wave;

    wv->shape_a = shape;
}

void WaveformDual_set_shape_b(WaveformDual *const wave,
                              e_Waveform_shape shape) {

    t_WaveformDual *wv = *wave;

    wv->shape_b = shape;
}

void WaveformDual_set_freq_a(WaveformDual *const wave, fract32 freq) {

    t_WaveformDual *wv = *wave;

    Phasor_set_freq(&wv->phasor_a, freq);
}

void WaveformDual_set_freq_b(WaveformDual *const wave, fract32 freq) {

    t_WaveformDual *wv = *wave;

    Phasor_set_freq(&wv->phasor_b, freq);
}

void WaveformDual_set_phase_a(WaveformDual *const wave, int32_t phase) {

    t_WaveformDual *wv = *wave;

    Phasor_set_phase(&wv->phasor_a, phase);
}

void WaveformDual_set_phase_b(WaveformDual *const wave, int32_t phase) {

    t_WaveformDual *wv = *wave;

    Phasor_set_phase(&wv->phasor_b, phase);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
