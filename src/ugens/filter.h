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

#include "ugens/interpolate.h"

/*----- Macros and Definitions ---------------------------------------*/

#define HPF_DEFAULT_FREQ (0)
#define LPF_DEFAULT_FREQ (20000 << 16)
#define BPF_DEFAULT_FREQ (2500 << 16)

typedef struct {
    Mempool mempool;
    fract32 lastIn;
    fract32 lastOut;
    fract32 freq;
} t_HPF;

typedef t_HPF *HPF;

typedef struct {
    Mempool mempool;
    fract32 lastOut;
    fract32 freq;
} t_LPF;

typedef t_LPF *LPF;

typedef struct {
    Mempool mempool;
    LPF lp;
    HPF hp;
    fract32 freq;
} t_BPF;

typedef t_BPF *BPF;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void HPF_init(HPF *const hpf, t_Aleph *const aleph);
void HPF_init_to_pool(HPF *const hpf, Mempool *const mempool);
void HPF_free(HPF *const hpf);

fract32 HPF_next(HPF *const hpf, fract32 in);
fract32 HPF_next_precise(HPF *const hpf, fract32 in);
fract32 HPF_next_dynamic(HPF *const hpf, fract32 in, fract32 freq);
fract32 HPF_next_dynamic_precise(HPF *const hpf, fract32 in, fract32 freq);

void LPF_init(LPF *const lpf, t_Aleph *const aleph);
void LPF_init_to_pool(LPF *const lpf, Mempool *const mempool);
void LPF_free(LPF *const lpf);

fract32 LPF_next(LPF *const lpf, fract32 in);
fract32 LPF_next_precise(LPF *const lpf, fract32 in);
fract32 LPF_next_dynamic(LPF *const lpf, fract32 in, fract32 freq);
fract32 LPF_next_dynamic_precise(LPF *const lpf, fract32 in, fract32 freq);

void BPF_init(BPF *const bpf, t_Aleph *const aleph);
void BPF_init_to_pool(BPF *const bpf, Mempool *const mempool);
void BPF_free(BPF *const bpf);

fract32 BPF_next(BPF *const bpf, fract32 in);
fract32 BPF_next_precise(BPF *const bpf, fract32 in);

fract32 BPF_next_dynamic(BPF *const bpf, fract32 in, fract32 hp_freq,
                         fract32 lp_freq);

fract32 BPF_next_dynamic_precise(BPF *const bpf, fract32 in, fract32 hp_freq,
                                 fract32 lp_freq);

fract32 HPF_dc_block(HPF *const hpf, fract32 in);
fract32 HPF_dc_block2(HPF *const hpf, fract32 in);

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
