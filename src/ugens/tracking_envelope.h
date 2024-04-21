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
    asymLinSlew slew;
} trackingEnvelopeLin;

typedef struct {
    fract32 val;
    fract32 up;
    fract32 down;
    fract32 gate;
} trackingEnvelopeLog;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void trackingEnvelopeLin_init(trackingEnvelopeLin *env);
fract32 trackingEnvelopeLin_next(trackingEnvelopeLin *env, fract32 in);
void trackingEnvelopeLog_init(trackingEnvelopeLog *env);
fract32 trackingEnvelopeLog_next(trackingEnvelopeLog *env, fract32 in);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
