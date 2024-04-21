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
 * @file    pitch_detector.h
 *
 * @brief   Public API for pitch detection.
 *
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include <stdint.h>

#include "filter.h"

/*----- Macros and Definitions ---------------------------------------*/

#define PITCH_DETECTOR_RADIX_INTERNAL 4
#define PITCH_DETECTOR_RADIX_EXTERNAL 8
#define PITCH_DETECTOR_RADIX_TOTAL                                             \
    (PITCH_DETECTOR_RADIX_INTERNAL + PITCH_DETECTOR_RADIX_EXTERNAL)

typedef struct {
    fract32 currentPeriod;
    fract32 lastIn;
    fract32 period;
    fract32 phase;
    int32_t nsamples;
    int32_t nFrames;
    t_HPF dcBlocker;
    t_LPF adaptiveFilter;
    fract32 pitchOffset;
} t_PitchDetector;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void PitchDetector_init(t_PitchDetector *p);
fract32 PitchDetector_track(t_PitchDetector *p, fract32 preIn);
fract32 PitchDetector_track_osc(t_PitchDetector *p);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
