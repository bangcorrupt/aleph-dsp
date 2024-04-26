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
 * @file    filter.h
 *
 * @brief   Public API for filters.
 *
 */

#ifndef ALEPH_FILTER_H
#define ALEPH_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "interpolate.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef struct {
    fract32 lastIn;
    fract32 lastOut;
} t_HPF;

typedef struct {
    fract32 lastOut;
} t_LPF;

typedef struct {
    t_LPF lp;
    t_HPF hp;
} t_BPF;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void HPF_init(t_HPF *hpf);
fract32 HPF_next_dynamic(t_HPF *hpf, fract32 in, fract32 freq);
fract32 HPF_next_dynamic_precise(t_HPF *hpf, fract32 in, fract32 freq);

void LPF_init(t_LPF *lpf);
fract32 LPF_next_dynamic(t_LPF *lpf, fract32 in, fract32 freq);
fract32 LPF_next_dynamic_precise(t_LPF *lpf, fract32 in, fract32 freq);

void BPF_init(t_BPF *bpf);
fract32 BPF_next_dynamic_precise(t_BPF *bpf, fract32 in, fract32 hp_freq,
                                 fract32 lp_freq);

fract32 HPF_dc_block(t_HPF *hpf, fract32 in);
fract32 HPF_dc_block2(t_HPF *hpf, fract32 in);

/*----- Static function implementations ------------------------------*/

/// TODO: Move these to filter.c if they are not used elsewhere.

static inline fract32 _hpf_freq_calc(fract32 freq) {
    // 1.0 / (2.0 * pi * dt * fc + 1.0)
    return one_over_x_16_16(
        add_fr1x32(mult_fr1x32x32(TWO_PI_16_16, freq), 1 << 16));
}

static inline fract32 _lpf_freq_calc(fract32 freq) {
    // 1.0 / ((1 / 2.0 * pi * dt * fc) + 1.0)
    fract32 temp = mult_fr1x32x32(TWO_PI_16_16, freq);
    return ((temp << 12) / ((1 << 16) + temp)) << 19;
}

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
