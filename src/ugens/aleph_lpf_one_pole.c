/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/
/*
 * @file    aleph_lpf_one_pole.c
 *
 * @brief   One pole low pass filter.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_lpf_one_pole.h"

/*----- Macros and Definitions ---------------------------------------*/

// A little under -120db
#define FR32_COMP_THRESH 0x4000

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

static inline uint8_t fr32_compare(fract32 a, fract32 b);

/*----- Extern function implementations ------------------------------*/

void Aleph_LPFOnePole_init(Aleph_LPFOnePole *const lpf, t_Aleph *const aleph) {

    Aleph_LPFOnePole_init_to_pool(lpf, &aleph->mempool);
}

void Aleph_LPFOnePole_init_to_pool(Aleph_LPFOnePole *const lpf,
                                   Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_LPFOnePole *lp = *lpf =
        (t_Aleph_LPFOnePole *)mpool_alloc(sizeof(t_Aleph_LPFOnePole), mp);

    lp->mempool = mp;

    lp->coeff = ALEPH_FILTER_ONE_POLE_DEFAULT_COEFF;
    lp->target = ALEPH_FILTER_ONE_POLE_DEFAULT_TARGET;
    lp->output = ALEPH_FILTER_ONE_POLE_DEFAULT_OUTPUT;
}

void Aleph_LPFOnePole_free(Aleph_LPFOnePole *const lpf) {

    t_Aleph_LPFOnePole *lp = *lpf;

    mpool_free((char *)lp, lp->mempool);
}

fract32 Aleph_LPFOnePole_next(Aleph_LPFOnePole *const lpf) {

    t_Aleph_LPFOnePole *lp = *lpf;

    lp->output = add_fr1x32(
        lp->target,
        mult_fr1x32x32(lp->coeff, sub_fr1x32(lp->output, lp->target)));

    return lp->output;
}

void Aleph_LPFOnePole_set_target(Aleph_LPFOnePole *const lpf, fract32 target) {

    t_Aleph_LPFOnePole *lp = *lpf;

    lp->target = target;
}

void Aleph_LPFOnePole_set_output(Aleph_LPFOnePole *const lpf, fract32 output) {

    t_Aleph_LPFOnePole *lp = *lpf;

    lp->target = output;
}

void Aleph_LPFOnePole_set_coeff(Aleph_LPFOnePole *const lpf, fract32 coeff) {

    t_Aleph_LPFOnePole *lp = *lpf;

    lp->coeff = coeff;
}

fract32 Aleph_LPFOnePole_norm_next(Aleph_LPFOnePole *const lpf) {

    t_Aleph_LPFOnePole *lp = *lpf;

    fract32 difference = sub_fr1x32(lp->output, lp->target);
    fract32 radix = norm_fr1x32(difference);

    lp->output = add_fr1x32(
        lp->target,
        shr_fr1x32(mult_fr1x32x32(lp->coeff, shl_fr1x32(difference, radix)),
                   radix));

    return lp->output;
}

bool Aleph_LPFOnePole_sync(Aleph_LPFOnePole *const lpf) {

    t_Aleph_LPFOnePole *lp = *lpf;

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
