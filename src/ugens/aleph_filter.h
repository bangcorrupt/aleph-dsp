/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/*
 * @file    aleph_filter.h
 *
 * @brief   Public API for ilters.
 *
 */

#ifndef ALEPH_FILTER_H
#define ALEPH_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_interpolate.h"

/*----- Macros -------------------------------------------------------*/

#define ALEPH_HPF_DEFAULT_FREQ (0)
#define ALEPH_LPF_DEFAULT_FREQ (20000 << 16)
#define ALEPH_BPF_DEFAULT_FREQ (2500 << 16)


/*----- Typedefs -----------------------------------------------------*/

typedef struct {
    Mempool mempool;
    fract32 last_in;
    fract32 last_out;
    fract32 freq;
} t_Aleph_HPF;

typedef t_Aleph_HPF *Aleph_HPF;

typedef struct {
    Mempool mempool;
    fract32 last_out;
    fract32 freq;
} t_Aleph_LPF;

typedef t_Aleph_LPF *Aleph_LPF;

typedef struct {
    Mempool mempool;
    Aleph_LPF lp;
    Aleph_HPF hp;
} t_Aleph_BPF;

typedef t_Aleph_BPF *Aleph_BPF;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_HPF_init(Aleph_HPF *const hpf, t_Aleph *const aleph);
void Aleph_HPF_init_to_pool(Aleph_HPF *const hpf, Mempool *const mempool);
void Aleph_HPF_free(Aleph_HPF *const hpf);

void Aleph_HPF_set_freq(Aleph_HPF *const hpf, fract32 freq);

fract32 Aleph_HPF_next(Aleph_HPF *const hpf, fract32 in);
fract32 Aleph_HPF_next_precise(Aleph_HPF *const hpf, fract32 in);
fract32 Aleph_HPF_next_dynamic(Aleph_HPF *const hpf, fract32 in, fract32 freq);
fract32 Aleph_HPF_next_dynamic_precise(Aleph_HPF *const hpf, fract32 in,
                                       fract32 freq);

void Aleph_LPF_init(Aleph_LPF *const lpf, t_Aleph *const aleph);
void Aleph_LPF_init_to_pool(Aleph_LPF *const lpf, Mempool *const mempool);
void Aleph_LPF_free(Aleph_LPF *const lpf);

void Aleph_LPF_set_freq(Aleph_LPF *const lpf, fract32 freq);

fract32 Aleph_LPF_next(Aleph_LPF *const lpf, fract32 in);
fract32 Aleph_LPF_next_precise(Aleph_LPF *const lpf, fract32 in);
fract32 Aleph_LPF_next_dynamic(Aleph_LPF *const lpf, fract32 in, fract32 freq);
fract32 Aleph_LPF_next_dynamic_precise(Aleph_LPF *const lpf, fract32 in,
                                       fract32 freq);

void Aleph_BPF_init(Aleph_BPF *const bpf, t_Aleph *const aleph);
void Aleph_BPF_init_to_pool(Aleph_BPF *const bpf, Mempool *const mempool);
void Aleph_BPF_free(Aleph_BPF *const bpf);

void Aleph_BPF_set_freq(Aleph_BPF *const bpf, fract32 hp_freq, fract32 lp_freq);

fract32 Aleph_BPF_next(Aleph_BPF *const bpf, fract32 in);
fract32 Aleph_BPF_next_precise(Aleph_BPF *const bpf, fract32 in);

fract32 Aleph_BPF_next_dynamic(Aleph_BPF *const bpf, fract32 in,
                               fract32 hp_freq, fract32 lp_freq);

fract32 Aleph_BPF_next_dynamic_precise(Aleph_BPF *const bpf, fract32 in,
                                       fract32 hp_freq, fract32 lp_freq);

fract32 Aleph_HPF_dc_block(Aleph_HPF *const hpf, fract32 in);
fract32 Aleph_HPF_dc_block2(Aleph_HPF *const hpf, fract32 in);

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
