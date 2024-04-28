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
 * @file    lpf_one_pole.c
 *
 * @brief   One pole low pass filter.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "lpf_one_pole.h"

/*----- Macros and Definitions ---------------------------------------*/

// A little under -120db
#define FR32_COMP_THRESH 0x4000

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

static inline uint8_t fr32_compare(fract32 a, fract32 b);

/*----- Extern function implementations ------------------------------*/

void LPFOnePole_init(LPFOnePole *const lpf, fract32 input,
                     t_Aleph *const aleph) {

    LPFOnePole_init_to_pool(lpf, input, &aleph->mempool);
}

void LPFOnePole_init_to_pool(LPFOnePole *const lpf, fract32 input,
                             Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_LPFOnePole *lp = *lpf =
        (t_LPFOnePole *)mpool_alloc(sizeof(t_LPFOnePole), mp);

    lp->mempool = mp;

    lp->coeff = FILTER_ONE_POLE_DEFAULT_COEFF;
    lp->target = input;
    lp->output = input;
}

fract32 LPFOnePole_free(LPFOnePole *const lpf) {

    t_LPFOnePole *lp = *lpf;

    mpool_free((char *)lp, lp->mempool);
}

fract32 LPFOnePole_next(LPFOnePole *const lpf) {

    t_LPFOnePole *lp = *lpf;

    lp->output = add_fr1x32(
        lp->target,
        mult_fr1x32x32(lp->coeff, sub_fr1x32(lp->output, lp->target)));

    return lp->output;
}

void LPFOnePole_set_input(LPFOnePole *const lpf, fract32 input) {

    t_LPFOnePole *lp = *lpf;

    lp->target = input;
}

void LPFOnePole_set_coeff(LPFOnePole *const lpf, fract32 coeff) {

    t_LPFOnePole *lp = *lpf;

    lp->coeff = coeff;
}

fract32 LPFOnePole_norm_next(LPFOnePole *const lpf) {

    t_LPFOnePole *lp = *lpf;

    fract32 difference = sub_fr1x32(lp->output, lp->target);
    fract32 radix = norm_fr1x32(difference);

    lp->output = add_fr1x32(
        lp->target,
        shr_fr1x32(mult_fr1x32x32(lp->coeff, shl_fr1x32(difference, radix)),
                   radix));

    return lp->output;
}

bool LPFOnePole_sync(LPFOnePole *const lpf) {

    t_LPFOnePole *lp = *lpf;

    // Early return if equal.
    if (lp->target == lp->output) {
        return 1;
    } else {
        if (fr32_compare(lp->target, lp->output)) {
            lp->output = lp->target;
            // Return 0, but next query will be true.
            return 0;
        } else {
            // Still converging.
            return 0;
        }
    }
}

/*----- Static function implementations ------------------------------*/

static inline uint8_t fr32_compare(fract32 a, fract32 b) {
    //  fract32 dif = sub_fr1x32(a, b);
    //  return ( ( dif < FR32_COMP_THRESH ) && ( dif > FR32_COMP_THRESH_NEG ) );
    return (abs_fr1x32(sub_fr1x32(a, b)) < FR32_COMP_THRESH);
}

/*----- END OF FILE --------------------------------------------------*/
