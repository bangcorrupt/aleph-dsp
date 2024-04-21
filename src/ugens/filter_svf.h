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
 * @file    filter_svf.h
 *
 * @brief   A digital state-variable filter for 32-bit fixed point audio.
 */

#ifndef ALEPH_FILTER_SVF_H
#define ALEPH_FILTER_SVF_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include <stdint.h>

#include "aleph.h"
#include "fix.h"
#include "types.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef struct {
    fract32 freq; // normalized frequency
    fract32 rq;   // reciprocal of q (resonance / bandwidth)
                  // range is [0, 2]
    fract32 low;  // lowpass
    fract32 band; // bandpass
    fract32 high;
    fract32 notch;

    // output mix
    fract32 lowMix;
    fract32 highMix;
    fract32 bandMix;
    fract32 notchMix;
    fract32 peakMix;

    // Kinda weird, but use rshift for rq values >=1
    uint8_t rqShift;

} t_FilterSVF;

typedef fract32 (*p_svf_func)(t_FilterSVF *f, fract32 in);

/*----- Extern variable declarations ---------------------------------*/

const extern p_svf_func FilterSVF_func[3][4];

/*----- Extern function prototypes -----------------------------------*/

// init
void FilterSVF_init(t_FilterSVF *f);
// set cutoff in hz
//  void t_FilterSVF_set_hz    ( t_FilterSVF* f, fix16 hz );
// set cutoff coefficient
void FilterSVF_set_coeff(t_FilterSVF *f, fract32 coeff);

// set RQ (reciprocal of q: resonance/bandwidth)
void FilterSVF_set_rq(t_FilterSVF *f, fract32 rq);
// set output mixes
void FilterSVF_set_low(t_FilterSVF *f, fract32 mix);
void FilterSVF_set_high(t_FilterSVF *f, fract32 mix);
void FilterSVF_set_band(t_FilterSVF *f, fract32 mix);
void FilterSVF_set_notch(t_FilterSVF *f, fract32 mix);
void FilterSVF_set_peak(t_FilterSVF *f, fract32 mix);
// get next value (with input)

fract32 FilterSVF_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_hpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_bpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_lpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_notch_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_softclip_hpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_softclip_bpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_softclip_lpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_softclip_notch_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_softclip_asym_lpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_softclip_asym_bpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_softclip_asym_hpf_next(t_FilterSVF *f, fract32 in);
fract32 FilterSVF_softclip_asym_notch_next(t_FilterSVF *f, fract32 in);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
