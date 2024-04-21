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

#ifndef FILTER_H
#define FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "fix.h"
#include "fract_math.h"
#include "types.h"

#include "interpolate.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef struct {
    fract32 lastIn;
    fract32 lastOut;
} hpf;

typedef struct {
    fract32 lastOut;
} lpf;

typedef struct {
    lpf lpf;
    hpf hpf;
} bpf;

/*----- Extern variable declarations ---------------------------------*/

/*----- Static function implementations ------------------------------*/

static inline fract32 hpf_freq_calc(fract32 freq) {
    // 1.0 / (2.0 * pi * dt * fc + 1.0)
    return one_over_x_16_16(
        add_fr1x32(mult_fr1x32x32(TWO_PI_16_16, freq), 1 << 16));
}

static inline fract32 lpf_freq_calc(fract32 freq) {
    // 1.0 / ((1 / 2.0 * pi * dt * fc) + 1.0)
    fract32 temp = mult_fr1x32x32(TWO_PI_16_16, freq);
    return ((temp << 12) / ((1 << 16) + temp)) << 19;
}

/*----- Extern function prototypes -----------------------------------*/

fract32 hpf_next_dynamic_precise(hpf *myHpf, fract32 in, fract32 freq);
void hpf_init(hpf *myHpf);
fract32 hpf_next_dynamic(hpf *myHpf, fract32 in, fract32 freq);

void lpf_init(lpf *myLpf);
fract32 lpf_next_dynamic(lpf *myLpf, fract32 in, fract32 freq);
fract32 lpf_next_dynamic_precise(lpf *myLpf, fract32 in, fract32 freq);

void bpf_init(bpf *myBpf);
fract32 bpf_next_dynamic_precise(bpf *myBpf, fract32 in, fract32 hpf_freq,
                                 fract32 lpf_freq);

fract32 dc_block(hpf *myHpf, fract32 in);
fract32 dc_block2(hpf *myHpf, fract32 in);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
