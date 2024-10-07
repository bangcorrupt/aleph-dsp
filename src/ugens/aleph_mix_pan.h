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
 * @file    aleph_mix_pan.h
 *
 * @brief   Public API for mixing and panning functions.
 */

#ifndef ALEPH_MIX_PAN_H
#define ALEPH_MIX_PAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

/// TODO: Check implementation.
//
// balanced, equal-power
// return panned sum of two inputs
// given pan coefficient in [0,1]
/* fract32 pan_bal_mix(fract32 inX, fract32 inY, fract32 pan); */

/// TODO: Check implementation.
//
// return just the pan coefficients
// given pan coefficient in [0,1]
/* void pan_bal_coeff(fract32 *a, fract32 *b, fract32 pan); */

/*----- Static function implementations ------------------------------*/

static inline void mix_aux_mono(fract32 in_mono, fract32 *out_left,
                                fract32 *out_right, fract32 left_value,
                                fract32 right_value) {

    *out_right = add_fr1x32(*out_right, mult_fr1x32x32(in_mono, right_value));
    *out_left = add_fr1x32(*out_left, mult_fr1x32x32(in_mono, left_value));
}

static inline void mix_panned_mono(fract32 in_mono, fract32 *out_left,
                                   fract32 *out_right, fract32 pan,
                                   fract32 fader) {
    fract32 pan_factor;
    fract32 post_fader;

    pan_factor = (fract32)(pan);
    post_fader = mult_fr1x32x32(pan_factor, fader);
    *out_left = add_fr1x32(*out_left, mult_fr1x32x32(in_mono, post_fader));

    pan_factor = (fract32)(FR32_MAX - pan);
    post_fader = mult_fr1x32x32(pan_factor, fader);
    *out_right = add_fr1x32(*out_right, mult_fr1x32x32(in_mono, post_fader));
}

//--- linear
// Simple linear pan
// return panned sum of two inputs
// given pan coefficient in [0,1]
static inline fract32 pan_lin_mix(fract32 inX, fract32 inY, fract32 pan) {

    fract32 ymul = pan;
    fract32 xmul = sub_fr1x32(FR32_MAX, ymul);
    return add_fr1x32(mult_fr1x32x32(inX, xmul), mult_fr1x32x32(inY, ymul));
}

static inline fract16 pan_lin_mix16(fract16 inX, fract16 inY, fract16 pan) {

    fract16 ymul = pan;
    fract16 xmul = sub_fr1x16(FR16_MAX, ymul);

    return add_fr1x16(multr_fr1x16(inX, xmul), multr_fr1x16(inY, ymul));
}

// return coefficients only
// given pan coefficient in [0,1]
static inline void pan_lin_coeff(fract32 *a, fract32 *b, fract32 pan) {
    // simple linear pan
    *a = pan;
    *b = sub_fr1x32(FR32_MAX, pan);
}

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
