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
 * @file    aleph_tracking_envelope.c
 *
 * @brief   Tracking envelope.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_interpolate.h"
#include "aleph_tracking_envelope.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_TrackingEnvLin_init(t_Aleph_TrackingEnvLin *env) {

    env->val = 0;

    Aleph_AsymLinSlew_init(&(env->slew), LINSLEW_10MS, LINSLEW_100MS);
}

fract32 Aleph_TrackingEnvLin_next(t_Aleph_TrackingEnvLin *env, fract32 in) {

    Aleph_AsymLinSlew_next(&(env->slew), &(env->val), abs_fr1x32(in));

    return env->val;
}

void Aleph_TrackingEnvLog_init(t_Aleph_TrackingEnvLog *env) {

    env->val = 0;
    env->up = SLEW_10MS;
    env->down = SLEW_100MS;
    env->gate = FR32_MAX / 500;
}

fract32 Aleph_TrackingEnvLog_next(t_Aleph_TrackingEnvLog *env, fract32 in) {

    fract32 target = abs_fr1x32(in);

    if (target > env->val) {
        SIMPLE_SLEW(env->val, target, env->up);

    } else if (target < env->val) {
        SIMPLE_SLEW(env->val, target, env->down);
    }

    return env->val;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
