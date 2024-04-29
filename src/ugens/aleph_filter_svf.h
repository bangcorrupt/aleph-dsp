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
 * @file    aleph_filter_svf.h
 *
 * @brief   Public API for digital state-variable filter.
 */

#ifndef ALEPH_FILTER_SVF_H
#define ALEPH_FILTER_SVF_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef enum {
    ALEPH_FILTERSVF_TYPE_LPF,
    ALEPH_FILTERSVF_TYPE_HPF,
    ALEPH_FILTERSVF_TYPE_BPF,
    ALEPH_FILTERSVF_TYPE_NOTCH,
} e_Aleph_FilterSVF_type;

typedef struct {

    t_Mempool *mempool;

    fract32 freq; // normalized frequency
    fract32 rq;   // reciprocal of q (resonance / bandwidth)
                  // range is [0, 2]
    fract32 low;  // lowpass
    fract32 high;
    fract32 band; // bandpass
    fract32 notch;

    // output mix
    fract32 low_mix;
    fract32 high_mix;
    fract32 band_mix;
    fract32 notch_mix;
    fract32 peak_mix;

    // Kinda weird, but use rshift for rq values >=1
    uint8_t rq_shift;

} t_Aleph_FilterSVF;

typedef t_Aleph_FilterSVF *Aleph_FilterSVF;

typedef fract32 (*p_Aleph_FilterSVF_func)(Aleph_FilterSVF *const filter,
                                          fract32 in);

/*----- Extern variable declarations ---------------------------------*/

const extern p_Aleph_FilterSVF_func Aleph_FilterSVF_func[3][4];

/*----- Extern function prototypes -----------------------------------*/

// init
void Aleph_FilterSVF_init(Aleph_FilterSVF *const filter, t_Aleph *const aleph);
void Aleph_FilterSVF_init_to_pool(Aleph_FilterSVF *const filter,
                                  Mempool *const mempool);
void Aleph_FilterSVF_free(Aleph_FilterSVF *const filter);
// set cutoff in hz
//  void t_Aleph_FilterSVF_set_hz    ( t_Aleph_FilterSVF* f, fix16 hz );
// set cutoff coefficient
void Aleph_FilterSVF_set_coeff(Aleph_FilterSVF *const filter, fract32 coeff);

// set RQ (reciprocal of q: resonance/bandwidth)
void Aleph_FilterSVF_set_rq(Aleph_FilterSVF *const filter, fract32 rq);
// set output mixes
void Aleph_FilterSVF_set_low(Aleph_FilterSVF *const filter, fract32 mix);
void Aleph_FilterSVF_set_high(Aleph_FilterSVF *const filter, fract32 mix);
void Aleph_FilterSVF_set_band(Aleph_FilterSVF *const filter, fract32 mix);
void Aleph_FilterSVF_set_notch(Aleph_FilterSVF *const filter, fract32 mix);
void Aleph_FilterSVF_set_peak(Aleph_FilterSVF *const filter, fract32 mix);
// get next value (with input)

fract32 Aleph_FilterSVF_next(Aleph_FilterSVF *const filter, fract32 in);
fract32 Aleph_FilterSVF_hpf_next(Aleph_FilterSVF *const filter, fract32 in);
fract32 Aleph_FilterSVF_bpf_next(Aleph_FilterSVF *const filter, fract32 in);
fract32 Aleph_FilterSVF_lpf_next(Aleph_FilterSVF *const filter, fract32 in);
fract32 Aleph_FilterSVF_notch_next(Aleph_FilterSVF *const filter, fract32 in);
fract32 Aleph_FilterSVF_softclip_hpf_next(Aleph_FilterSVF *const filter,
                                          fract32 in);
fract32 Aleph_FilterSVF_softclip_bpf_next(Aleph_FilterSVF *const filter,
                                          fract32 in);
fract32 Aleph_FilterSVF_softclip_lpf_next(Aleph_FilterSVF *const filter,
                                          fract32 in);
fract32 Aleph_FilterSVF_softclip_notch_next(Aleph_FilterSVF *const filter,
                                            fract32 in);
fract32 Aleph_FilterSVF_softclip_asym_lpf_next(Aleph_FilterSVF *const filter,
                                               fract32 in);
fract32 Aleph_FilterSVF_softclip_asym_bpf_next(Aleph_FilterSVF *const filter,
                                               fract32 in);
fract32 Aleph_FilterSVF_softclip_asym_hpf_next(Aleph_FilterSVF *const filter,
                                               fract32 in);
fract32 Aleph_FilterSVF_softclip_asym_notch_next(Aleph_FilterSVF *const filter,
                                                 fract32 in);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
