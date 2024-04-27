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

#include "aleph-mempool.h"
#include "filter.h"
#include "oscillator.h"

#include "pitch_detector.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void PitchDetector_init(PitchDetector *const pitch_detect,
                        t_Aleph *const aleph) {

    PitchDetector_init_to_pool(pitch_detect, &aleph->mempool);
}

void PitchDetector_init_to_pool(PitchDetector *const pitch_detect,
                                Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_PitchDetector *pd = *pitch_detect =
        (t_PitchDetector *)mpool_alloc(sizeof(t_PitchDetector), mp);

    pd->current_period = 48 << PITCH_DETECTOR_RADIX_TOTAL;
    pd->period = 48 << PITCH_DETECTOR_RADIX_TOTAL;
    pd->last_in = 1;
    pd->phase = 0;
    pd->nsamples = 100;
    pd->nframes = 100;
    pd->pitch_offset = FR32_MAX >> 2;

    HPF_init_to_pool(&(pd->dcblocker), mempool);
    LPF_init_to_pool(&(pd->adaptive_filter), mempool);
}

// This guy returns the current measured wave period (in subsamples)
fract32 PitchDetector_track_next(PitchDetector *const pitch_detect,
                                 fract32 in) {

    t_PitchDetector *pd = *pitch_detect;

    fract32 centreFreq = FR32_MAX / pd->current_period;

    in = LPF_next_dynamic_precise(
        &(pd->adaptive_filter), in,
        shl_fr1x32(centreFreq, PITCH_DETECTOR_RADIX_INTERNAL + 3));

    in = HPF_next_dynamic_precise(
        &(pd->dcblocker), in,
        shl_fr1x32(centreFreq, PITCH_DETECTOR_RADIX_INTERNAL - 3));

    if (pd->last_in <= 0 && in >= 0 && pd->nframes > 12) {
        pd->period = add_fr1x32(pd->period, min_fr1x32(pd->nframes, 2048));
        pd->nframes = 0;
        pd->nsamples += 1;
        if (pd->nsamples >= (1 << PITCH_DETECTOR_RADIX_INTERNAL)) {
            pd->current_period = pd->period;
            pd->period = 0;
            pd->nsamples = 0;
        }
    }
    pd->nframes += 1;
    pd->last_in = in;

    return (shl_fr1x32(pd->current_period, PITCH_DETECTOR_RADIX_EXTERNAL -
                                               PITCH_DETECTOR_RADIX_INTERNAL));
}

/// TODO: Select waveform shape.
//
fract32 PitchDetector_osc_next(PitchDetector *const pitch_detect) {

    t_PitchDetector *pd = *pitch_detect;

    // Debug uncomment the line below to force 1k tone
    /* pd->current_period = (48 << (PITCH_DETECTOR_RADIX_INTERNAL)); */

    pd->phase += (pd->pitch_offset / shl_fr1x32(pd->current_period,
                                                -PITCH_DETECTOR_RADIX_INTERNAL))
                 << 3;

    return osc_sin(pd->phase);
}

/// TODO: Convenience function to track and return oscillator in one call.

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
