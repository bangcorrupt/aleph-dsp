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
 * @file    ricks_tricks.h
 *
 * @brief   Utilities.
 *
 */

#ifndef ALEPH_RICKS_TRICKS_H
#define ALEPH_RICKS_TRICKS_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "fix.h"
#include "fract_math.h"
#include "types.h"

/*----- Macros and Definitions ---------------------------------------*/

#define TWOPI 6
#define PI 3
#define TWO_PI_16_16 411775

#define SR 48000

// freq_to_phase in 2.30
#define FREQ_TO_PHASE_CONST 0x57619F00

#define NORMALISED_FREQUENCY(hz) ((fract32)((fract32)hz * (FR32_MAX / SR)))
#define NORMALISED_FREQUENCY16(hz) ((fract16)((fract16)hz * (FR16_MAX / SR)))

#define FADE_OUT(x) sub_fr1x32(FR32_MAX, mult_fr1x32x32(x, x))
#define FADE_IN(x) FADE_OUT(sub_fr1x32(FR32_MAX, x))

/*----- Extern variable declarations ---------------------------------*/

#define FR32_1_OVER_3 (FR32_MAX / 3)
#define FR16_1_OVER_3 (FR16_MAX / 3)

/*----- Extern function prototypes -----------------------------------*/

/*----- Static function implementations ------------------------------*/

// Takes a fraction in 16_16 radix, returns it in 0_32
static inline fract32 one_over_x_16_16(fract32 x_16_16) {
    return shl_fr1x32(FR32_MAX / x_16_16, 16);
}

// convert fix16 frequency to normalized fract32 phase increment
static inline fract32 freq_to_phase(fix16 freq) {
    u8 rad = norm_fr1x32(freq);
    freq = shl_fr1x32(freq, rad);
    fract32 ret = mult_fr1x32x32(freq, FREQ_TO_PHASE_CONST);
    return shr_fr1x32(ret, rad - 1);
}

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
