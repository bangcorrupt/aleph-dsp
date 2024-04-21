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

#include "fix.h"
#include "fract_math.h"
#include "types.h"

#include "interpolate.h"

#include "filter.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void hpf_init(hpf *myHpf) {
    myHpf->lastIn = 0;
    myHpf->lastOut = 0;
}

void lpf_init(lpf *myLpf) { myLpf->lastOut = 0; }

void bpf_init(bpf *myBpf) {
    hpf_init(&(myBpf->hpf));
    lpf_init(&(myBpf->lpf));
}

fract32 hpf_next_dynamic_precise(hpf *myHpf, fract32 in, fract32 freq) {
    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = hpf_freq_calc(freq);
    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, myHpf->lastOut),
                   mult_fr1x32x32(alpha, (sub_fr1x32(in, myHpf->lastIn))));
    myHpf->lastOut = out;
    myHpf->lastIn = in;
    return out;
}

fract32 hpf_next_dynamic(hpf *myHpf, fract32 in, fract32 freq) {
    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = freq * 4;
    /* alpha = (FR32_MAX / 50); */
    myHpf->lastOut =
        add_fr1x32(mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), myHpf->lastOut),
                   mult_fr1x32x32(alpha, sub_fr1x32(in, myHpf->lastIn)));
    myHpf->lastIn = in;
    return myHpf->lastOut * (FR32_MAX / freq / 4);
}

// the frequency unit is fraction of samplerate
fract32 lpf_next_dynamic(lpf *myLpf, fract32 in, fract32 freq) {
    return simple_slew(myLpf->lastOut, in, TWOPI * freq);
}

fract32 lpf_next_dynamic_precise(lpf *myLpf, fract32 in, fract32 freq) {
    fract32 alpha = lpf_freq_calc(freq);
    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, in),
                   mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), myLpf->lastOut));
    myLpf->lastOut = out;
    return out;
}

fract32 bpf_next_dynamic_precise(bpf *myBpf, fract32 in, fract32 hpf_freq,
                                 fract32 lpf_freq) {
    return lpf_next_dynamic_precise(
        &(myBpf->lpf), hpf_next_dynamic_precise(&(myBpf->hpf), in, hpf_freq),
        lpf_freq);
}

fract32 dc_block(hpf *myHpf, fract32 in) {
    fract32 in_scaled = shr_fr1x32(in, 3);
    fract32 aux = sub_fr1x32(in_scaled, myHpf->lastIn);
    myHpf->lastOut =
        add_fr1x32(aux, mult_fr1x32x32(0x7F600000, myHpf->lastOut));
    myHpf->lastIn = in_scaled;
    return shl_fr1x32(myHpf->lastOut, 3);
}

fract32 dc_block2(hpf *myHpf, fract32 in) {
    fract32 in_scaled = shr_fr1x32(in, 3);
    myHpf->lastOut = mult_fr1x32x32(
        sub_fr1x32(add_fr1x32(in_scaled, myHpf->lastOut), myHpf->lastIn),
        0x7F600000);
    myHpf->lastIn = in_scaled;
    return shl_fr1x32(myHpf->lastOut, 3);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
