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
 * @file    aleph_soft_clip.c
 *
 * @brief   Soft clipping.
 *
 */

/// TODO: Maybe fold this into another module or move back to ricks_tricks.

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_soft_clip.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

fract32 soft_clip(fract32 lim, fract32 in) {
    fract32 b = FR32_MAX / shr_fr1x32(FR32_MAX - lim, 24);
    fract32 a = negate_fr1x32(shr_fr1x32(b, 1));
    fract32 c = mult_fr1x32x32(lim, shr_fr1x32(FR32_MAX, 7) - b -
                                        mult_fr1x32x32(lim, a));
    if (in >= lim) {
        fract32 ret = c;
        ret = add_fr1x32(ret, mult_fr1x32x32(b, in));
        ret = add_fr1x32(ret, mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
        return shl_fr1x32(ret, 7);
    } else if (in <= negate_fr1x32(lim)) {
        fract32 ret = negate_fr1x32(c);
        ret = add_fr1x32(ret, mult_fr1x32x32(b, in));
        ret = sub_fr1x32(ret, mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
        return shl_fr1x32(ret, 7);
    } else {
        return in;
    }
}

fract32 soft_clip_asym(fract32 lim_pos, fract32 lim_neg, fract32 in) {
    fract32 lim;
    if (in >= lim_pos) {
        lim = lim_pos;
        fract32 b = FR32_MAX / shr_fr1x32(FR32_MAX - lim, 24);
        fract32 a = negate_fr1x32(shr_fr1x32(b, 1));
        fract32 c = mult_fr1x32x32(lim, shr_fr1x32(FR32_MAX, 7) - b -
                                            mult_fr1x32x32(lim, a));

        fract32 ret = c;
        ret = add_fr1x32(ret, mult_fr1x32x32(b, in));
        ret = add_fr1x32(ret, mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
        return shl_fr1x32(ret, 7);
    } else if (in <= lim_neg) {
        lim = negate_fr1x32(lim_neg);
        fract32 b = FR32_MAX / shr_fr1x32(FR32_MAX - lim, 24);
        fract32 a = negate_fr1x32(shr_fr1x32(b, 1));
        fract32 c = mult_fr1x32x32(lim, shr_fr1x32(FR32_MAX, 7) - b -
                                            mult_fr1x32x32(lim, a));

        fract32 ret = negate_fr1x32(c);
        ret = add_fr1x32(ret, mult_fr1x32x32(b, in));
        ret = sub_fr1x32(ret, mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
        return shl_fr1x32(ret, 7);
    } else {
        return in;
    }
}

fract32 soft_clip_norm(fract32 lim, fract32 in) {
    fract32 b = FR32_MAX / shr_fr1x32(FR32_MAX - lim, 24);
    fract32 a = negate_fr1x32(shr_fr1x32(b, 1));
    fract32 c = mult_fr1x32x32(lim, shr_fr1x32(FR32_MAX, 7) - b -
                                        mult_fr1x32x32(lim, a));
    fract32 norm = FR32_MAX / shr_fr1x32(a + b + c, 20);
    if (in >= lim) {
        fract32 ret = c;
        ret = add_fr1x32(ret, mult_fr1x32x32(b, in));
        ret = add_fr1x32(ret, mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
        return shl_fr1x32(mult_fr1x32x32(ret, norm), 11);
    } else if (in <= negate_fr1x32(lim)) {
        fract32 ret = negate_fr1x32(c);
        ret = add_fr1x32(ret, mult_fr1x32x32(b, in));
        ret = sub_fr1x32(ret, mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
        return shl_fr1x32(mult_fr1x32x32(ret, norm), 11);
    } else {
        return shl_fr1x32(mult_fr1x32x32(in, norm), 4);
    }
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
