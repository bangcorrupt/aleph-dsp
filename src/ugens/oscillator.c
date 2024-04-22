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

#include "fix.h"
#include "fract_math.h"
#include "types.h"

#include "oscillator.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

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

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
