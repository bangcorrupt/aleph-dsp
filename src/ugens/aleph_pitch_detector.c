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
 * @file    aleph_pitch_detector.c
 *
 * @brief   Pitch detection.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_filter.h"
#include "aleph_oscillator.h"

#include "aleph_pitch_detector.h"

/*----- Macros -------------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_PitchDetector_init(Aleph_PitchDetector *const pitch_detect,
                              t_Aleph *const aleph) {

    Aleph_PitchDetector_init_to_pool(pitch_detect, &aleph->mempool);
}

void Aleph_PitchDetector_init_to_pool(Aleph_PitchDetector *const pitch_detect,
                                      Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_PitchDetector *pd = *pitch_detect =
        (t_Aleph_PitchDetector *)mpool_alloc(sizeof(t_Aleph_PitchDetector), mp);

    pd->current_period = 48 << ALEPH_PITCH_DETECTOR_RADIX_TOTAL;
    pd->period = 48 << ALEPH_PITCH_DETECTOR_RADIX_TOTAL;
    pd->last_in = 1;
    pd->phase = 0;
    pd->nsamples = 100;
    pd->nframes = 100;
    pd->pitch_offset = FR32_MAX >> 2;

    Aleph_HPF_init_to_pool(&(pd->dcblocker), mempool);
    Aleph_LPF_init_to_pool(&(pd->adaptive_filter), mempool);
}

// This guy returns the current measured wave period (in subsamples)
fract32 Aleph_PitchDetector_track_next(Aleph_PitchDetector *const pitch_detect,
                                       fract32 in) {

    t_Aleph_PitchDetector *pd = *pitch_detect;

    fract32 centreFreq = FR32_MAX / pd->current_period;

    in = Aleph_LPF_next_dynamic_precise(
        &(pd->adaptive_filter), in,
        shl_fr1x32(centreFreq, ALEPH_PITCH_DETECTOR_RADIX_INTERNAL + 3));

    in = Aleph_HPF_next_dynamic_precise(
        &(pd->dcblocker), in,
        shl_fr1x32(centreFreq, ALEPH_PITCH_DETECTOR_RADIX_INTERNAL - 3));

    if (pd->last_in <= 0 && in >= 0 && pd->nframes > 12) {
        pd->period = add_fr1x32(pd->period, min_fr1x32(pd->nframes, 2048));
        pd->nframes = 0;
        pd->nsamples += 1;
        if (pd->nsamples >= (1 << ALEPH_PITCH_DETECTOR_RADIX_INTERNAL)) {
            pd->current_period = pd->period;
            pd->period = 0;
            pd->nsamples = 0;
        }
    }
    pd->nframes += 1;
    pd->last_in = in;

    return (shl_fr1x32(pd->current_period,
                       ALEPH_PITCH_DETECTOR_RADIX_EXTERNAL -
                           ALEPH_PITCH_DETECTOR_RADIX_INTERNAL));
}

/// TODO: Select waveform shape.
//
fract32 Aleph_PitchDetector_osc_next(Aleph_PitchDetector *const pitch_detect) {

    t_Aleph_PitchDetector *pd = *pitch_detect;

    // Debug uncomment the line below to force 1k tone
    /* pd->current_period = (48 << (ALEPH_PITCH_DETECTOR_RADIX_INTERNAL)); */

    pd->phase +=
        (pd->pitch_offset /
         shl_fr1x32(pd->current_period, -ALEPH_PITCH_DETECTOR_RADIX_INTERNAL))
        << 3;

    return osc_sin(pd->phase);
}

/// TODO: Convenience function to track and return oscillator in one call.

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
