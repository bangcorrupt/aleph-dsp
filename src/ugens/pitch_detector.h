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

#include "aleph-mempool.h"
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
    Mempool mempool;
    HPF dcblocker;
    LPF adaptive_filter;
    fract32 current_period;
    fract32 last_in;
    fract32 period;
    fract32 phase;
    int32_t nsamples;
    int32_t nframes;
    fract32 pitch_offset;
} t_PitchDetector;

typedef t_PitchDetector *PitchDetector;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void PitchDetector_init(PitchDetector *const pitch_detect,
                        t_Aleph *const aleph);

void PitchDetector_init_to_pool(PitchDetector *const pitch_detect,
                                Mempool *const mempool);

void PitchDetector_free(PitchDetector *const pitch_detect);

fract32 PitchDetector_track_next(PitchDetector *const pitch_detect,
                                 fract32 pre_in);

fract32 PitchDetector_osc_next(PitchDetector *const pitch_detect);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
