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
 * @file    tracking_envelope.h
 *
 * @brief   Public API for tracking envelope.
 *
 */

#ifndef ALEPH_TRACKING_ENVELOPE_H
#define ALEPH_TRACKING_ENVELOPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "fix.h"
#include "fract_math.h"
#include "types.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef struct {
    fract32 val;
    t_AsymLinSlew slew;

} t_TrackingEnvLin;

typedef struct {
    fract32 val;
    fract32 up;
    fract32 down;
    fract32 gate;

} t_TrackingEnvLog;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void TrackingEnvLin_init(t_TrackingEnvLin *env);
fract32 TrackingEnvLin_next(t_TrackingEnvLin *env, fract32 in);

void TrackingEnvLog_init(t_TrackingEnvLog *env);
fract32 TrackingEnvLog_next(t_TrackingEnvLog *env, fract32 in);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
