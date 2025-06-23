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
 * @file    aleph_waveform.c
 *
 * @brief   Basic waveform oscillators using PolyBLEP.
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_mempool.h"
#include "aleph_osc_polyblep.h"
#include "aleph_phasor.h"
#include "aleph_waveform.h"
#include "fract_typedef.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_Waveform_init(Aleph_Waveform *const wave, t_Aleph *const aleph) {

    Aleph_Waveform_init_to_pool(wave, &aleph->mempool);
}

void Aleph_Waveform_init_to_pool(Aleph_Waveform *const wave,
                                 Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_Waveform *wv = *wave =
        (t_Aleph_Waveform *)mpool_alloc(sizeof(t_Aleph_Waveform), mp);

    wv->mempool = mp;

    wv->shape = WAVEFORM_SHAPE_SINE;

    // Aleph_Phasor_init(&wv->phasor, mp->leaf);
    Aleph_Phasor_init_to_pool(&wv->phasor, mempool);
    Aleph_Phasor_set_freq(&wv->phasor, WAVEFORM_DEFAULT_FREQ);
    Aleph_Phasor_set_phase(&wv->phasor, WAVEFORM_DEFAULT_PHASE);
}

void Aleph_Waveform_free(Aleph_Waveform *const wave) {

    t_Aleph_Waveform *wv = *wave;

    Aleph_Phasor_free(&wv->phasor);

    mpool_free((char *)wv, wv->mempool);
}

fract32 Aleph_Waveform_next(Aleph_Waveform *const wave) {

    t_Aleph_Waveform *wv = *wave;

    fract32 next;

    Aleph_Phasor_next(&wv->phasor);

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

void Aleph_Waveform_set_shape(Aleph_Waveform *const wave,
                              e_Aleph_Waveform_shape shape) {

    t_Aleph_Waveform *wv = *wave;

    wv->shape = shape;
}

void Aleph_Waveform_set_freq(Aleph_Waveform *const wave, fract32 freq) {

    t_Aleph_Waveform *wv = *wave;

    Aleph_Phasor_set_freq(&wv->phasor, freq);
}

void Aleph_Waveform_set_phase(Aleph_Waveform *const wave, int32_t phase) {

    t_Aleph_Waveform *wv = *wave;

    Aleph_Phasor_set_phase(&wv->phasor, phase);
}

void Aleph_WaveformDual_init(Aleph_WaveformDual *const wave,
                             t_Aleph *const aleph) {

    Aleph_WaveformDual_init_to_pool(wave, &aleph->mempool);
}

void Aleph_WaveformDual_init_to_pool(Aleph_WaveformDual *const wave,
                                     Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_WaveformDual *wv = *wave =
        (t_Aleph_WaveformDual *)mpool_alloc(sizeof(t_Aleph_WaveformDual), mp);

    wv->mempool = mp;

    wv->shape_a = WAVEFORM_SHAPE_SINE;
    wv->shape_b = WAVEFORM_SHAPE_SINE;

    Aleph_Phasor_init_to_pool(&wv->phasor_a, mempool);
    Aleph_Phasor_set_freq(&wv->phasor_a, WAVEFORM_DEFAULT_FREQ);
    Aleph_Phasor_set_phase(&wv->phasor_a, WAVEFORM_DEFAULT_PHASE);

    Aleph_Phasor_init_to_pool(&wv->phasor_b, mempool);
    Aleph_Phasor_set_freq(&wv->phasor_b, WAVEFORM_DEFAULT_FREQ);
    Aleph_Phasor_set_phase(&wv->phasor_b, WAVEFORM_DEFAULT_PHASE);
}

void Aleph_WaveformDual_free(Aleph_WaveformDual *const wave) {

    t_Aleph_WaveformDual *wv = *wave;

    Aleph_Phasor_free(&wv->phasor_a);
    Aleph_Phasor_free(&wv->phasor_b);

    mpool_free((char *)wv, wv->mempool);
}

fract32 Aleph_WaveformDual_next(Aleph_WaveformDual *const wave) {

    t_Aleph_WaveformDual *wv = *wave;

    fract32 next_a;
    fract32 next_b;

    Aleph_Phasor_next(&wv->phasor_a);
    Aleph_Phasor_next(&wv->phasor_b);

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

void Aleph_WaveformDual_next_block(Aleph_WaveformDual *const wave,
                                   fract32 *output, size_t size) {

    t_Aleph_WaveformDual *wv = *wave;

    // In this case, we can use the same buffer for phase and polyblep output.
    fract32 *next_a =
        (fract32 *)mpool_alloc(size * sizeof(fract32), wv->mempool);

    fract32 *next_b =
        (fract32 *)mpool_alloc(size * sizeof(fract32), wv->mempool);

    Aleph_Phasor_next_block(&wv->phasor_a, next_a, size);
    Aleph_Phasor_next_block(&wv->phasor_b, next_b, size);

    switch (wv->shape_a) {

    case WAVEFORM_SHAPE_SINE:
        sine_polyblep_block(next_a, next_a, size);
        break;

    case WAVEFORM_SHAPE_TRIANGLE:
        triangle_polyblep_block(next_a, next_a, size);
        break;

    case WAVEFORM_SHAPE_SAW:
        saw_polyblep_block(next_a, wv->phasor_a->freq, next_a, size);
        break;

    case WAVEFORM_SHAPE_SQUARE:
        square_polyblep_block(next_a, wv->phasor_a->freq, next_a, size);
        break;

    default:
        sine_polyblep_block(next_a, next_a, size);
        break;
    }

    switch (wv->shape_b) {

    case WAVEFORM_SHAPE_SINE:
        sine_polyblep_block(next_b, next_b, size);
        break;

    case WAVEFORM_SHAPE_TRIANGLE:
        triangle_polyblep_block(next_b, next_b, size);
        break;

    case WAVEFORM_SHAPE_SAW:
        saw_polyblep_block(next_b, wv->phasor_a->freq, next_b, size);
        break;

    case WAVEFORM_SHAPE_SQUARE:
        square_polyblep_block(next_b, wv->phasor_a->freq, next_b, size);
        break;

    default:
        sine_polyblep_block(next_b, next_b, size);
        break;
    }

    int i;
    for (i = 0; i < size; i++) {

        output[i] =
            add_fr1x32(shl_fr1x32(next_a[i], 15), shl_fr1x32(next_b[i], 15));
    }

    mpool_free((char *)next_a, wv->mempool);
    mpool_free((char *)next_b, wv->mempool);
}

void Aleph_WaveformDual_next_block_smooth(Aleph_WaveformDual *const wave,
                                          fract32 *freq, fract32 *output,
                                          size_t size) {

    t_Aleph_WaveformDual *wv = *wave;

    // In this case, we can use the same buffer for phase and polyblep output.
    fract32 *next_a =
        (fract32 *)mpool_alloc(size * sizeof(fract32), wv->mempool);

    fract32 *next_b =
        (fract32 *)mpool_alloc(size * sizeof(fract32), wv->mempool);

    /// TODO: `freq_offset` needs to be a parameter of `WaveformDual`.
    ///        Is `WaveformDual` any better than just using two `Waveform`?
    //
    Aleph_Phasor_next_block_smooth(&wv->phasor_a, freq, next_a, size);

    /// TODO: `freq_offset`

    Aleph_Phasor_next_block_smooth(&wv->phasor_b, freq, next_b, size);

    switch (wv->shape_a) {

    case WAVEFORM_SHAPE_SINE:
        sine_polyblep_block(next_a, next_a, size);
        break;

    case WAVEFORM_SHAPE_TRIANGLE:
        triangle_polyblep_block(next_a, next_a, size);
        break;

    case WAVEFORM_SHAPE_SAW:
        saw_polyblep_block_smooth(next_a, freq, next_a, size);
        break;

    case WAVEFORM_SHAPE_SQUARE:
        square_polyblep_block_smooth(next_a, freq, next_a, size);
        break;

    default:
        sine_polyblep_block(next_a, next_a, size);
        break;
    }

    switch (wv->shape_b) {

    case WAVEFORM_SHAPE_SINE:
        sine_polyblep_block(next_b, next_b, size);
        break;

    case WAVEFORM_SHAPE_TRIANGLE:
        triangle_polyblep_block(next_b, next_b, size);
        break;

    case WAVEFORM_SHAPE_SAW:
        saw_polyblep_block_smooth(next_b, freq, next_b, size);
        break;

    case WAVEFORM_SHAPE_SQUARE:
        square_polyblep_block_smooth(next_b, freq, next_b, size);
        break;

    default:
        sine_polyblep_block(next_b, next_b, size);
        break;
    }

    int i;
    for (i = 0; i < size; i++) {

        output[i] =
            add_fr1x32(shl_fr1x32(next_a[i], 15), shl_fr1x32(next_b[i], 15));
    }

    mpool_free((char *)next_a, wv->mempool);
    mpool_free((char *)next_b, wv->mempool);
}

void Aleph_WaveformDual_set_shape(Aleph_WaveformDual *const wave,
                                  e_Aleph_Waveform_shape shape) {

    t_Aleph_WaveformDual *wv = *wave;

    wv->shape_a = shape;
    wv->shape_b = shape;
}

void Aleph_WaveformDual_set_freq(Aleph_WaveformDual *const wave, fract32 freq) {

    t_Aleph_WaveformDual *wv = *wave;

    Aleph_Phasor_set_freq(&wv->phasor_a, freq);
    Aleph_Phasor_set_freq(&wv->phasor_b, freq);
}

void Aleph_WaveformDual_set_phase(Aleph_WaveformDual *const wave,
                                  int32_t phase) {

    t_Aleph_WaveformDual *wv = *wave;

    Aleph_Phasor_set_phase(&wv->phasor_a, phase);
    Aleph_Phasor_set_phase(&wv->phasor_b, phase);
}

void Aleph_WaveformDual_set_shape_a(Aleph_WaveformDual *const wave,
                                    e_Aleph_Waveform_shape shape) {

    t_Aleph_WaveformDual *wv = *wave;

    wv->shape_a = shape;
}

void Aleph_WaveformDual_set_shape_b(Aleph_WaveformDual *const wave,
                                    e_Aleph_Waveform_shape shape) {

    t_Aleph_WaveformDual *wv = *wave;

    wv->shape_b = shape;
}

void Aleph_WaveformDual_set_freq_a(Aleph_WaveformDual *const wave,
                                   fract32 freq) {

    t_Aleph_WaveformDual *wv = *wave;

    Aleph_Phasor_set_freq(&wv->phasor_a, freq);
}

void Aleph_WaveformDual_set_freq_b(Aleph_WaveformDual *const wave,
                                   fract32 freq) {

    t_Aleph_WaveformDual *wv = *wave;

    Aleph_Phasor_set_freq(&wv->phasor_b, freq);
}

void Aleph_WaveformDual_set_phase_a(Aleph_WaveformDual *const wave,
                                    int32_t phase) {

    t_Aleph_WaveformDual *wv = *wave;

    Aleph_Phasor_set_phase(&wv->phasor_a, phase);
}

void Aleph_WaveformDual_set_phase_b(Aleph_WaveformDual *const wave,
                                    int32_t phase) {

    t_Aleph_WaveformDual *wv = *wave;

    Aleph_Phasor_set_phase(&wv->phasor_b, phase);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
