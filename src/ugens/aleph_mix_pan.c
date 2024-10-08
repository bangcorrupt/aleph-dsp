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
 * @file    aleph_mix_pan.c
 *
 * @brief   Mixing and panning functions.
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_mix_pan.h"

/*----- Macros -------------------------------------------------------*/

#define PAN_SINE_BUF_SIZE 1024
#define PAN_SINE_BUF_SIZE_MASK 1023
// shift down from unipolar fract32 to get 16.16 index
#define PAN_SINE_BUF_RSHIFT 5
#define PAN_SINE_BUF_SIZE_FIX (1024 << 16)
#define PAN_SINE_BUF_MAX16 (1024 * FIX16_ONE - 1)

/*
static fract32 sineTab[PAN_SINE_BUF_SIZE] = {
#include "halfsine_1024.inc"
};
*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

// Equal-power balanced pan

fract32 pan_bal_mix(fract32 inX, fract32 inY, fract32 pan) {
    /// TODO: Does this work?
    /*
    //  fix16 yIdx = fix16_mul_fract(FRACT_FIX16(pan), PAN_SINE_BUF_MAX16);
    // shift to get 16.16 idx
    fix16 yIdx = pan >> PAN_SINE_BUF_RSHIFT;
    fix16 xIdx = fix16_sub(PAN_SINE_BUF_MAX16, yIdx);
    fract32 xmul = table_lookup_idx_mask(sineTab, PAN_SINE_BUF_MASK, xIdx);
    fract32 ymul = table_lookup_idx(sineTab, PAN_SINE_BUF_MASK, yIdx);
    return add_fr1x32( mult_fr1x32x32(inX, xmul), mult_fr1x32x32(inY, ymul) );
    */
    return 0;
}

void pan_bal_coeff(fract32 *a, fract32 *b, fract32 pan) {
    /// TODO: Does this work?
    /*
    fix16 yIdx = pan >> PAN_SINE_BUF_RSHIFT;
    fix16 xIdx = fix16_sub(PAN_SINE_BUF_MAX16, yIdx);
    *a = table_lookup_idx_mask(sineTab, PAN_SINE_BUF_MASK, xIdx);
    *b = table_lookup_idx(sineTab, PAN_SINE_BUF_MASK, yIdx);
    */
    /* fix16 yIdx = fix16_mul_fract(FRACT_FIX16(pan), PAN_SINE_BUF_MAX16); */
    /* fix16 xIdx = fix16_sub(PAN_SINE_BUF_MAX16, yIdx); */
    /* *a = table_lookup_idx(sineTab, PAN_SINE_BUF_SIZE, xIdx); */
    /* *b = table_lookup_idx(sineTab, PAN_SINE_BUF_SIZE, yIdx); */
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
