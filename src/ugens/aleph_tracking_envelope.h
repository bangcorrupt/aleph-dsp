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

/*
 * @file    aleph_tracking_envelope.h
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

#include "aleph.h"

#include "aleph_interpolate.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef struct {
    fract32 val;
    t_Aleph_AsymLinSlew slew;

} t_Aleph_TrackingEnvLin;

typedef struct {
    fract32 val;
    fract32 up;
    fract32 down;
    fract32 gate;

} t_Aleph_TrackingEnvLog;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_TrackingEnvLin_init(t_Aleph_TrackingEnvLin *env);
fract32 Aleph_TrackingEnvLin_next(t_Aleph_TrackingEnvLin *env, fract32 in);

void Aleph_TrackingEnvLog_init(t_Aleph_TrackingEnvLog *env);
fract32 Aleph_TrackingEnvLog_next(t_Aleph_TrackingEnvLog *env, fract32 in);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
