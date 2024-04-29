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
 * @file    aleph_pitch_detector.h
 *
 * @brief   Public API for pitch detection.
 *
 */

#ifndef ALEPH_PITCH_DETECTOR_H
#define ALEPH_PITCH_DETECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_filter.h"

/*----- Macros and Definitions ---------------------------------------*/

#define ALEPH_PITCH_DETECTOR_RADIX_INTERNAL 4
#define ALEPH_PITCH_DETECTOR_RADIX_EXTERNAL 8
#define ALEPH_PITCH_DETECTOR_RADIX_TOTAL                                       \
    (ALEPH_PITCH_DETECTOR_RADIX_INTERNAL + ALEPH_PITCH_DETECTOR_RADIX_EXTERNAL)

typedef struct {
    Mempool mempool;
    Aleph_HPF dcblocker;
    Aleph_LPF adaptive_filter;
    fract32 current_period;
    fract32 last_in;
    fract32 period;
    fract32 phase;
    int32_t nsamples;
    int32_t nframes;
    fract32 pitch_offset;
} t_Aleph_PitchDetector;

typedef t_Aleph_PitchDetector *Aleph_PitchDetector;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_PitchDetector_init(Aleph_PitchDetector *const pitch_detect,
                              t_Aleph *const aleph);

void Aleph_PitchDetector_init_to_pool(Aleph_PitchDetector *const pitch_detect,
                                      Mempool *const mempool);

void Aleph_PitchDetector_free(Aleph_PitchDetector *const pitch_detect);

fract32 Aleph_PitchDetector_track_next(Aleph_PitchDetector *const pitch_detect,
                                       fract32 pre_in);

fract32 Aleph_PitchDetector_osc_next(Aleph_PitchDetector *const pitch_detect);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
