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
 * @file    tracking_envelope.c
 *
 * @brief   Tracking envelope.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "fix.h"
#include "fract_math.h"
#include "types.h"

#include "interpolate.h"
#include "tracking_envelope.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void trackingEnvelopeLin_init(trackingEnvelopeLin *env) {
    env->val = 0;
    asymLinSlew_init(&(env->slew));
}

fract32 trackingEnvelopeLin_next(trackingEnvelopeLin *env, fract32 in) {
    asymLinSlew_next(&(env->val), abs_fr1x32(in), &(env->slew));
    return env->val;
}

void trackingEnvelopeLog_init(trackingEnvelopeLog *env) {
    env->val = 0;
    env->up = SLEW_10MS;
    env->down = SLEW_100MS;
    env->gate = FR32_MAX / 500;
}

fract32 trackingEnvelopeLog_next(trackingEnvelopeLog *env, fract32 in) {
    fract32 target = abs_fr1x32(in);
    if (target > env->val)
        simple_slew(env->val, target, env->up);
    else if (target < env->val)
        simple_slew(env->val, target, env->down);
    return env->val;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/