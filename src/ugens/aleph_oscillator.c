/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by monome, modified by bangcorrupt 2024. */

/*
 * @file    aleph_oscillator.c
 *
 * @brief   Oscillators.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_phasor.h"

#include "aleph_oscillator.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_Oscillator_init(Aleph_Oscillator *const oscillator,
                           t_Aleph *const aleph) {

    Aleph_Oscillator_init_to_pool(oscillator, &aleph->mempool);
}

void Aleph_Oscillator_init_to_pool(Aleph_Oscillator *const oscillator,
                                   Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_Oscillator *osc = *oscillator =
        (t_Aleph_Oscillator *)mpool_alloc(sizeof(t_Aleph_Oscillator), mp);

    osc->mempool = mp;

    Aleph_Phasor_init_to_pool(&osc->phasor, mempool);
    Aleph_Phasor_set_freq(&osc->phasor, ALEPH_OSCILLATOR_DEFAULT_FREQ);
    Aleph_Phasor_set_phase(&osc->phasor, ALEPH_OSCILLATOR_DEFAULT_PHASE);
}

void Aleph_Oscillator_free(Aleph_Oscillator *const oscillator) {

    t_Aleph_Oscillator *osc = *oscillator;

    Aleph_Phasor_free(&osc->phasor);

    mpool_free((char *)osc, osc->mempool);
}

fract32 Aleph_Oscillator_next(Aleph_Oscillator *const oscillator) {

    t_Aleph_Oscillator *osc = *oscillator;

    fract32 next;

    Aleph_Phasor_next(&osc->phasor);

    switch (osc->shape) {

    case ALEPH_OSCILLATOR_SHAPE_SINE:
        next = osc_sin(osc->phasor->phase);
        break;

    case ALEPH_OSCILLATOR_SHAPE_TRIANGLE:
        next = osc_triangle(osc->phasor->phase);
        break;

    case ALEPH_OSCILLATOR_SHAPE_SAW:

        /// TODO: Is this bipolar?
        next = osc->phasor->phase;
        break;

    case ALEPH_OSCILLATOR_SHAPE_SQUARE:
        next = osc_square(osc->phasor->phase);
        break;

    default:
        next = 0;
        break;
    }

    return next;
}

fract16 Aleph_Oscillator_16_next(Aleph_Oscillator *const oscillator) {

    t_Aleph_Oscillator *osc = *oscillator;

    fract32 next;

    Aleph_Phasor_next(&osc->phasor);

    switch (osc->shape) {

    case ALEPH_OSCILLATOR_SHAPE_SINE:
        next = osc_sin16(osc->phasor->phase);
        break;

    case ALEPH_OSCILLATOR_SHAPE_TRIANGLE:
        next = osc_triangle16(osc->phasor->phase);
        break;

    case ALEPH_OSCILLATOR_SHAPE_SAW:

        /// TODO: Is this bipolar?
        next = trunc_fr1x32(osc->phasor->phase);
        break;

    case ALEPH_OSCILLATOR_SHAPE_SQUARE:
        next = osc_square16(osc->phasor->phase);
        break;

    default:
        next = 0;
        break;
    }

    return next;
}

void Aleph_Oscillator_set_shape(Aleph_Oscillator *const oscillator,
                                e_Aleph_Oscillator_shape shape) {

    t_Aleph_Oscillator *osc = *oscillator;

    osc->shape = shape;
}

void Aleph_Oscillator_set_freq(Aleph_Oscillator *const oscillator,
                               fract32 freq) {

    t_Aleph_Oscillator *osc = *oscillator;

    Aleph_Phasor_set_freq(&osc->phasor, freq);
}

void Aleph_Oscillator_set_phase(Aleph_Oscillator *const oscillator,
                                int32_t phase) {

    t_Aleph_Oscillator *osc = *oscillator;

    Aleph_Phasor_set_phase(&osc->phasor, phase);
}

fract32 osc_sin(fract32 phase) {
    if (phase > (1 << 30) || phase < (-1 << 30)) {
        phase = FR32_MIN - phase;
        return sub_fr1x32(shl_fr1x32(mult_fr1x32x32(phase, phase), 2),
                          FR32_MAX);
    } else if (phase < (1 << 30) || phase <= (-1 << 30)) {
        return sub_fr1x32(FR32_MAX,
                          shl_fr1x32(mult_fr1x32x32(phase, phase), 2));
    } else
        return 0;
}

fract16 osc_sin16(fract32 phase) {
    fract16 phase16;
    if (phase > (1 << 30) || phase < (-1 << 30)) {
        phase16 = FR16_MIN - trunc_fr1x32(phase);
        return sub_fr1x16(shl_fr1x16(multr_fr1x16(phase16, phase16), 2),
                          FR16_MAX);
    } else if (phase < (1 << 30) || phase <= (-1 << 30)) {
        phase16 = trunc_fr1x32(phase);
        return sub_fr1x16(FR16_MAX,
                          shl_fr1x16(multr_fr1x16(phase16, phase16), 2));
    } else
        return 0;
}

fract32 osc_triangle(fract32 phase) {

    if (phase > (1 << 30) || phase < (-1 << 30)) {
        phase = FR32_MIN - phase;
        return sub_fr1x32(shl_fr1x32(abs_fr1x32(phase), 1), FR32_MAX);

    } else if (phase < (1 << 30) || phase <= (-1 << 30))
        return sub_fr1x32(FR32_MAX, shl_fr1x32(abs_fr1x32(phase), 1));

    else
        return 0;
}

fract16 osc_triangle16(fract32 phase) {

    fract16 phase16;

    if (phase > (1 << 30) || phase < (-1 << 30)) {
        phase16 = FR16_MIN - trunc_fr1x32(phase);
        return sub_fr1x16(shl_fr1x16(abs_fr1x16(phase16), 1), FR16_MAX);

    } else if (phase < (1 << 30) || phase <= (-1 << 30)) {
        phase16 = trunc_fr1x32(phase);
        return sub_fr1x32(FR16_MAX, shl_fr1x16(abs_fr1x16(phase16), 1));

    } else
        return 0;
}

fract32 osc_square(fract32 phase) {
    if (phase > (1 << 30) || phase < (-1 << 30))
        return FR32_MAX;
    else
        return FR32_MIN;
}

fract16 osc_square16(fract32 phase) {
    if (phase > (1 << 30) || phase < (-1 << 30))
        return FR16_MAX;
    else
        return FR16_MIN;
}

/// TODO: Are these bipolar?
fract32 osc_saw(fract32 phase) { return phase; }

fract16 osc_saw16(fract32 phase) { return trunc_fr1x32(phase); }

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
