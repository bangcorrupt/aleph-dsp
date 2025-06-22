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

/**
 * @file    aleph_osc_polyblep.c
 *
 * @brief   Oscillator polyblep.
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_phasor.h"
#include "aleph_utils.h"
#include "fract_typedef.h"
#include "types.h"
#include <stddef.h>

#include "aleph_osc_polyblep.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

static inline fix16 _polyblep(fract32 p, fract32 dp);
static inline void _polyblep_block(fract32 dp, fract32 *buffer, size_t size);

/*----- Extern function implementations ------------------------------*/

fract16 square_polyblep(fract32 p, fract32 dp) {

    fix16 square_raw = 0xFFFF;
    fix16 square_pb;

    if (p < 0) {

        square_raw *= -1;
    }

    square_pb = add_fr1x32(square_raw, _polyblep(p + FR32_MAX, dp));
    square_pb = sub_fr1x32(square_pb, _polyblep(p, dp));

    return (fract16)shr_fr1x32(square_pb, 1);
}

void square_polyblep_block(fract32 *phase, fract32 freq, fract32 *output,
                           size_t size) {

    fix16 square_raw;
    fix16 square_pb;

    int i;
    for (i = 0; i < size; i++) {

        square_raw = 0xFFFF;

        if (phase[i] < 0) {

            square_raw *= -1;
        }

        square_pb =
            add_fr1x32(square_raw, _polyblep(phase[i] + FR32_MAX, freq));

        square_pb = sub_fr1x32(square_pb, _polyblep(phase[i], freq));

        output[i] = (fract16)shr_fr1x32(square_pb, 1);
    }
}

fract16 saw_polyblep(fract32 p, fract32 dp) {

    return shr_fr1x32(sub_fr1x32(shr_fr1x32(p, 15), _polyblep(p, dp)), 1);
}

void saw_polyblep_block(fract32 *phase, fract32 freq, fract32 *output,
                        size_t size) {

    int i;
    for (i = 0; i < size; i++) {

        output[i] = shr_fr1x32(
            sub_fr1x32(shr_fr1x32(phase[i], 15), _polyblep(phase[i], freq)), 1);
    }
}

void saw_polyblep_block_smooth(fract32 *phase, fract32 *freq, fract32 *output,
                               size_t size) {

    int i;
    for (i = 0; i < size; i++) {

        output[i] = shr_fr1x32(
            sub_fr1x32(shr_fr1x32(phase[i], 15), _polyblep(phase[i], freq[i])),
            1);
    }
}

fract16 sine_polyblep(fract32 phase) {

    fract16 phase16;

    phase -= FR32_MAX / 2;

    if (phase > (1 << 30) || phase < (-1 << 30)) {

        phase16 = FR16_MIN - trunc_fr1x32(phase);

        return sub_fr1x16(shl_fr1x16(multr_fr1x16(phase16, phase16), 2),
                          FR16_MAX);

    } else if (phase < (1 << 30) || phase <= (-1 << 30)) {

        phase16 = trunc_fr1x32(phase);

        return sub_fr1x16(FR16_MAX,
                          shl_fr1x16(multr_fr1x16(phase16, phase16), 2));

    } else {
        return 0;
    }
}

void sine_polyblep_block(fract32 *phase, fract32 *output, size_t size) {

    fract32 ph;
    fract16 phase16;

    int i;
    for (i = 0; i < size; i++) {

        ph = phase[i];
        ph -= FR32_MAX / 2;

        if (ph > (1 << 30) || ph < (-1 << 30)) {

            phase16 = FR16_MIN - trunc_fr1x32(ph);

            output[i] = sub_fr1x16(
                shl_fr1x16(multr_fr1x16(phase16, phase16), 2), FR16_MAX);

        } else if (ph < (1 << 30) || ph <= (-1 << 30)) {

            phase16 = trunc_fr1x32(ph);

            output[i] = sub_fr1x16(
                FR16_MAX, shl_fr1x16(multr_fr1x16(phase16, phase16), 2));

        } else {
            output[i] = 0;
        }
    }
}

fract16 triangle_polyblep(fract32 phase) {
    fract16 phase16;
    phase -= FR32_MAX / 2;
    if (phase > (1 << 30) || phase < (-1 << 30)) {
        phase16 = FR16_MIN - trunc_fr1x32(phase);
        return sub_fr1x16(shl_fr1x16(abs_fr1x16(phase16), 1), FR16_MAX);
    } else if (phase < (1 << 30) || phase <= (-1 << 30)) {
        phase16 = trunc_fr1x32(phase);
        return sub_fr1x32(FR16_MAX, shl_fr1x16(abs_fr1x16(phase16), 1));
    } else
        return 0;
}

void triangle_polyblep_block(fract32 *phase, fract32 *output, size_t size) {

    fract32 ph;
    fract16 phase16;

    int i;
    for (i = 0; i < size; i++) {

        ph = phase[i];
        ph -= FR32_MAX / 2;

        if (ph > (1 << 30) || ph < (-1 << 30)) {

            phase16 = FR16_MIN - trunc_fr1x32(ph);

            output[i] =
                sub_fr1x16(shl_fr1x16(abs_fr1x16(phase16), 1), FR16_MAX);

        } else if (phase < (1 << 30) || ph <= (-1 << 30)) {

            phase16 = trunc_fr1x32(ph);

            output[i] =
                sub_fr1x32(FR16_MAX, shl_fr1x16(abs_fr1x16(phase16), 1));

        } else {
            output[i] = 0;
        }
    }
}

/*----- Static function implementations ------------------------------*/

static inline fix16 _polyblep(fract32 p, fract32 dp) {

    fix16 dp_inv = FR32_MAX / shr_fr1x32(dp, 16);

    fix16 p_by_dp = fix16_mul_fract(dp_inv, shr_fr1x32(p, 15));
    fix16 p_plus_one_by_dp = dp_inv + p_by_dp;
    fix16 one_minus_p_by_dp = dp_inv - p_by_dp;

    if (p < add_fr1x32(dp, FR32_MIN)) {

        return shl_fr1x32(p_plus_one_by_dp, 1) -
               fix16_mul_fract_radix(p_plus_one_by_dp, p_plus_one_by_dp, 15) -
               0x10000;
    }

    if (p > sub_fr1x32(FR32_MAX, dp)) {

        return negate_fr1x32(
            shl_fr1x32(one_minus_p_by_dp, 1) -
            fix16_mul_fract_radix(one_minus_p_by_dp, one_minus_p_by_dp, 15) -
            0x10000);
    }

    return 0;
}

static inline void _polyblep_block(fract32 dp, fract32 *output, size_t size) {

    fract32 p;

    fix16 p_by_dp;
    fix16 p_plus_one_by_dp;
    fix16 one_minus_p_by_dp;

    fix16 dp_inv = FR32_MAX / shr_fr1x32(dp, 16);

    int i;
    for (i = 0; i < size; i++) {

        p = output[i];

        p_by_dp = fix16_mul_fract(dp_inv, shr_fr1x32(p, 15));
        p_plus_one_by_dp = dp_inv + p_by_dp;
        one_minus_p_by_dp = dp_inv - p_by_dp;

        if (p < add_fr1x32(dp, FR32_MIN)) {

            output[i] =
                shl_fr1x32(p_plus_one_by_dp, 1) -
                fix16_mul_fract_radix(p_plus_one_by_dp, p_plus_one_by_dp, 15) -
                0x10000;

        } else if (p > sub_fr1x32(FR32_MAX, dp)) {

            output[i] =
                negate_fr1x32(shl_fr1x32(one_minus_p_by_dp, 1) -
                              fix16_mul_fract_radix(one_minus_p_by_dp,
                                                    one_minus_p_by_dp, 15) -
                              0x10000);
        } else {

            output[i] = 0;
        }
    }
}

/*----- End of file --------------------------------------------------*/
