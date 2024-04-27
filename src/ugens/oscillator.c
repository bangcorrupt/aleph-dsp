/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/Aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by monome, modified by bangcorrupt 2024. */

/*
 * @file    oscillators.c
 *
 * @brief   Oscillators.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "ugens/oscillator.h"
#include "ugens/phasor.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Oscillator_init(Oscillator *const oscillator, t_Aleph *const aleph) {

    Oscillator_init_to_pool(oscillator, &aleph->mempool);
}

void Oscillator_init_to_pool(Oscillator *const oscillator,
                             Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Oscillator *osc = *oscillator =
        (t_Oscillator *)mpool_alloc(sizeof(t_Oscillator), mp);

    osc->mempool = mp;

    Phasor_init_to_pool(&osc->phasor, mempool);
    Phasor_set_freq(&osc->phasor, OSCILLATOR_DEFAULT_FREQ);
    Phasor_set_phase(&osc->phasor, OSCILLATOR_DEFAULT_PHASE);
}

void Oscillator_free(Oscillator *const oscillator) {

    t_Oscillator *osc = *oscillator;

    Phasor_free(&osc->phasor);

    mpool_free((char *)osc, osc->mempool);
}

fract32 Oscillator_next(Oscillator *const oscillator) {

    t_Oscillator *osc = *oscillator;

    fract32 next;

    Phasor_next(&osc->phasor);

    switch (osc->shape) {

    case OSCILLATOR_SHAPE_SINE:
        next = osc_sin(osc->phasor->phase);
        break;

    case OSCILLATOR_SHAPE_TRIANGLE:
        next = osc_triangle(osc->phasor->phase);
        break;

        // case OSCILLATOR_SHAPE_SAW:
        //     /// TODO:  Make saw bipolar.
        //
        //     next = osc->phasor->phase;
        //     break;

    case OSCILLATOR_SHAPE_SQUARE:
        next = osc_square(osc->phasor->phase);
        break;

    default:
        next = 0;
        break;
    }

    return next;
}

fract16 Oscillator_16_next(Oscillator *const oscillator) {

    t_Oscillator *osc = *oscillator;

    fract32 next;

    Phasor_next(&osc->phasor);

    switch (osc->shape) {

    case OSCILLATOR_SHAPE_SINE:
        next = osc_sin16(osc->phasor->phase);
        break;

    case OSCILLATOR_SHAPE_TRIANGLE:
        next = osc_triangle16(osc->phasor->phase);
        break;

        // case OSCILLATOR_SHAPE_SAW:
        //     /// TODO:  Make saw bipolar.
        //
        //     next = osc->phasor->phase;
        //     break;

    case OSCILLATOR_SHAPE_SQUARE:
        next = osc_square16(osc->phasor->phase);
        break;

    default:
        next = 0;
        break;
    }

    return next;
}

void Oscillator_set_shape(Oscillator *const oscillator,
                          e_Oscillator_shape shape) {

    t_Oscillator *osc = *oscillator;

    osc->shape = shape;
}

void Oscillator_set_freq(Oscillator *const oscillator, fract32 freq) {

    t_Oscillator *osc = *oscillator;

    Phasor_set_freq(&osc->phasor, freq);
}

void Oscillator_set_phase(Oscillator *const oscillator, int32_t phase) {

    t_Oscillator *osc = *oscillator;

    Phasor_set_phase(&osc->phasor, phase);
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

/// TODO: osc_saw
fract32 osc_saw(fract32 phase) {
    //
}

fract32 osc_saw16(fract32 phase) {
    //
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
