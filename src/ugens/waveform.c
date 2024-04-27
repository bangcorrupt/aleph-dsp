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

void Waveform_init(t_Waveform *wave, t_Aleph *aleph) {
    //
    Waveform_init_to_pool(wave, aleph->mempool);
}

void Waveform_init_to_pool(t_Waveform *wave, t_Mempool *mempool) {

    t_Mempool *mp = mempool;

    wave = (t_Waveform *)mpool_alloc(sizeof(t_Waveform), mp);
    wave->mempool = mp;
    // wave->mempool->aleph = mempool->aleph;

    wave->shape = WAVEFORM_SHAPE_SINE;

    Phasor_init(wave->phasor, mempool->aleph);
    Phasor_set_freq(wave->phasor, WAVEFORM_DEFAULT_FREQ);
}

fract32 Waveform_next(t_Waveform *wave) {

    fract32 next;

    switch (wave->shape) {

    case WAVEFORM_SHAPE_SINE:
        next = sine_polyblep(wave->phasor->phase);
        break;

    case WAVEFORM_SHAPE_TRIANGLE:
        next = triangle_polyblep(wave->phasor->phase);
        break;

    case WAVEFORM_SHAPE_SAW:
        next = saw_polyblep(wave->phasor->phase, wave->phasor->freq);
        break;

    case WAVEFORM_SHAPE_SQUARE:
        next = square_polyblep(wave->phasor->phase, wave->phasor->freq);
        break;

    default:
        next = 0;
        break;
    }

    return shl_fr1x32(next, 16);
}

void Waveform_set_shape(t_Waveform *wave, uint8_t shape) {
    //
    wave->shape = shape;
}

void Waveform_set_freq(t_Waveform *wave, fract32 freq) {
    //
    Phasor_set_freq(wave->phasor, freq);
}

void Waveform_set_phase(t_Waveform *wave, int32_t phase) {
    //
    Phasor_set_phase(wave->phasor, phase);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
