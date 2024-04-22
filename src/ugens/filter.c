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
 * @file    filter.c
 *
 * @brief   Filters.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "interpolate.h"

#include "filter.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void HPF_init(t_HPF *hpf) {
    hpf->lastIn = 0;
    hpf->lastOut = 0;
}

void LPF_init(t_LPF *lpf) { lpf->lastOut = 0; }

void BPF_init(t_BPF *bpf) {
    HPF_init(&(bpf->hp));
    LPF_init(&(bpf->lp));
}

fract32 HPF_next_dynamic_precise(t_HPF *hpf, fract32 in, fract32 freq) {
    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = _hpf_freq_calc(freq);
    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, hpf->lastOut),
                   mult_fr1x32x32(alpha, (sub_fr1x32(in, hpf->lastIn))));
    hpf->lastOut = out;
    hpf->lastIn = in;
    return out;
}

fract32 HPF_next_dynamic(t_HPF *hpf, fract32 in, fract32 freq) {
    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = freq * 4;
    /* alpha = (FR32_MAX / 50); */
    hpf->lastOut =
        add_fr1x32(mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), hpf->lastOut),
                   mult_fr1x32x32(alpha, sub_fr1x32(in, hpf->lastIn)));
    hpf->lastIn = in;
    return hpf->lastOut * (FR32_MAX / freq / 4);
}

// the frequency unit is fraction of samplerate
fract32 LPF_next_dynamic(t_LPF *lpf, fract32 in, fract32 freq) {
    return SIMPLE_SLEW(lpf->lastOut, in, TWOPI * freq);
}

fract32 LPF_next_dynamic_precise(t_LPF *lpf, fract32 in, fract32 freq) {
    fract32 alpha = _lpf_freq_calc(freq);
    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, in),
                   mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), lpf->lastOut));
    lpf->lastOut = out;
    return out;
}

fract32 BPF_next_dynamic_precise(t_BPF *bpf, fract32 in, fract32 hp_freq,
                                 fract32 lp_freq) {
    return LPF_next_dynamic_precise(
        &(bpf->lp), HPF_next_dynamic_precise(&(bpf->hp), in, hp_freq), lp_freq);
}

fract32 HPF_dc_block(t_HPF *hpf, fract32 in) {
    fract32 in_scaled = shr_fr1x32(in, 3);
    fract32 aux = sub_fr1x32(in_scaled, hpf->lastIn);
    hpf->lastOut = add_fr1x32(aux, mult_fr1x32x32(0x7F600000, hpf->lastOut));
    hpf->lastIn = in_scaled;
    return shl_fr1x32(hpf->lastOut, 3);
}

fract32 HPF_dc_block2(t_HPF *hpf, fract32 in) {
    fract32 in_scaled = shr_fr1x32(in, 3);
    hpf->lastOut = mult_fr1x32x32(
        sub_fr1x32(add_fr1x32(in_scaled, hpf->lastOut), hpf->lastIn),
        0x7F600000);
    hpf->lastIn = in_scaled;
    return shl_fr1x32(hpf->lastOut, 3);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
