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
 * @file    pitch_detector.c
 *
 * @brief   Pitch detection.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "filter.h"
#include "oscillator.h"

#include "pitch_detector.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void PitchDetector_init(t_PitchDetector *p) {
    p->currentPeriod = 48 << PITCH_DETECTOR_RADIX_TOTAL;
    p->period = 48 << PITCH_DETECTOR_RADIX_TOTAL;
    p->lastIn = 1;
    p->phase = 0;
    p->nsamples = 100;
    p->nFrames = 100;
    p->pitchOffset = FR32_MAX >> 2;
    /* p->pitchOffset = FR32_MAX / 2; */
    HPF_init(&(p->dcBlocker));
    LPF_init(&(p->adaptiveFilter));
}

// This guy returns the current measured wave period (in subsamples)
fract32 PitchDetector_track(t_PitchDetector *p, fract32 in) {
    fract32 centreFreq = FR32_MAX / p->currentPeriod;
    in = LPF_next_dynamic_precise(
        &(p->adaptiveFilter), in,
        shl_fr1x32(centreFreq, PITCH_DETECTOR_RADIX_INTERNAL + 3));
    in = HPF_next_dynamic_precise(
        &(p->dcBlocker), in,
        shl_fr1x32(centreFreq, PITCH_DETECTOR_RADIX_INTERNAL - 3));
    if (p->lastIn <= 0 && in >= 0 && p->nFrames > 12) {
        p->period = add_fr1x32(p->period, min_fr1x32(p->nFrames, 2048));
        p->nFrames = 0;
        p->nsamples += 1;
        if (p->nsamples >= (1 << PITCH_DETECTOR_RADIX_INTERNAL)) {
            p->currentPeriod = p->period;
            p->period = 0;
            p->nsamples = 0;
        }
    }
    p->nFrames += 1;
    p->lastIn = in;
    return (shl_fr1x32(p->currentPeriod, PITCH_DETECTOR_RADIX_EXTERNAL -
                                             PITCH_DETECTOR_RADIX_INTERNAL));
}

/// TODO: Select waveform shape.
//
fract32 PitchDetector_track_osc(t_PitchDetector *p) {
    // Debug uncomment the line below to force 1k tone
    /* p->currentPeriod = (48 << (PITCH_DETECTOR_RADIX_INTERNAL)); */
    p->phase += (p->pitchOffset /
                 shl_fr1x32(p->currentPeriod, -PITCH_DETECTOR_RADIX_INTERNAL))
                << 3;
    return osc_sin(p->phase);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
