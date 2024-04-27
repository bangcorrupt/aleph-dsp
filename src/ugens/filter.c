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

#include "aleph-mempool.h"
#include "aleph.h"

#include "ugens/interpolate.h"

#include "ugens/filter.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void HPF_init(HPF *const hpf, t_Aleph *const aleph) {

    HPF_init_to_pool(hpf, &aleph->mempool);
}

void HPF_init_to_pool(HPF *const hpf, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_HPF *hp = *hpf = (t_HPF *)mpool_alloc(sizeof(t_HPF), mp);

    hp->mempool = mp;

    hp->freq = HPF_DEFAULT_FREQ;
    hp->lastIn = 0;
    hp->lastOut = 0;
}

void HPF_free(HPF *const hpf) {

    t_HPF *hp = *hpf;

    mpool_free((char *)hp, hp->mempool);
}

fract32 HPF_set_freq(HPF *const hpf, fract32 freq) {

    t_HPF *hp = *hpf;

    hp->freq = freq;
}

fract32 HPF_next(HPF *const hpf, fract32 in) {

    t_HPF *hp = *hpf;

    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = hp->freq * 4;

    hp->lastOut =
        add_fr1x32(mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), hp->lastOut),
                   mult_fr1x32x32(alpha, sub_fr1x32(in, hp->lastIn)));

    hp->lastIn = in;

    return hp->lastOut * (FR32_MAX / hp->freq / 4);
}

fract32 HPF_next_precise(HPF *const hpf, fract32 in) {

    t_HPF *hp = *hpf;

    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = _hpf_freq_calc(hp->freq);

    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, hp->lastOut),
                   mult_fr1x32x32(alpha, (sub_fr1x32(in, hp->lastIn))));

    hp->lastOut = out;
    hp->lastIn = in;

    return out;
}

fract32 HPF_next_dynamic(HPF *const hpf, fract32 in, fract32 freq) {

    t_HPF *hp = *hpf;

    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = freq * 4;

    hp->lastOut =
        add_fr1x32(mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), hp->lastOut),
                   mult_fr1x32x32(alpha, sub_fr1x32(in, hp->lastIn)));

    hp->lastIn = in;

    return hp->lastOut * (FR32_MAX / freq / 4);
}

fract32 HPF_next_dynamic_precise(HPF *const hpf, fract32 in, fract32 freq) {

    t_HPF *hp = *hpf;

    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = _hpf_freq_calc(freq);

    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, hp->lastOut),
                   mult_fr1x32x32(alpha, (sub_fr1x32(in, hp->lastIn))));

    hp->lastOut = out;
    hp->lastIn = in;

    return out;
}

void LPF_init(LPF *const lpf, t_Aleph *const aleph) {

    LPF_init_to_pool(lpf, &aleph->mempool);
}

void LPF_init_to_pool(LPF *const lpf, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_LPF *lp = *lpf = (t_LPF *)mpool_alloc(sizeof(t_LPF), mp);

    lp->mempool = mp;

    lp->freq = LPF_DEFAULT_FREQ;
    lp->lastOut = 0;
}

void LPF_free(LPF *const lpf) {

    t_LPF *lp = *lpf;

    mpool_free((char *)lp, lp->mempool);
}

fract32 LPF_set_freq(LPF *const lpf, fract32 freq) {

    t_LPF *lp = *lpf;

    lp->freq = freq;
}

// the frequency unit is fraction of samplerate
fract32 LPF_next(LPF *const lpf, fract32 in) {

    t_LPF *lp = *lpf;

    return SIMPLE_SLEW(lp->lastOut, in, TWOPI * lp->freq);
}

fract32 LPF_next_precise(LPF *const lpf, fract32 in) {

    t_LPF *lp = *lpf;

    fract32 alpha = _lpf_freq_calc(lp->freq);

    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, in),
                   mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), lp->lastOut));

    lp->lastOut = out;

    return out;
}

// the frequency unit is fraction of samplerate
fract32 LPF_next_dynamic(LPF *const lpf, fract32 in, fract32 freq) {

    t_LPF *lp = *lpf;

    return SIMPLE_SLEW(lp->lastOut, in, TWOPI * freq);
}

fract32 LPF_next_dynamic_precise(LPF *const lpf, fract32 in, fract32 freq) {

    t_LPF *lp = *lpf;

    fract32 alpha = _lpf_freq_calc(freq);

    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, in),
                   mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), lp->lastOut));

    lp->lastOut = out;

    return out;
}

void BPF_init(BPF *const bpf, t_Aleph *const aleph) {

    BPF_init_to_pool(bpf, &aleph->mempool);
}

void BPF_init_to_pool(BPF *const bpf, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_BPF *bp = *bpf = (t_BPF *)mpool_alloc(sizeof(t_BPF), mp);

    bp->mempool = mp;

    HPF_init_to_pool(&bp->hp, mempool);
    LPF_init_to_pool(&bp->lp, mempool);
}

void BPF_free(BPF *const bpf) {

    t_BPF *bp = *bpf;

    HPF_free(&bp->hp);
    LPF_free(&bp->lp);

    mpool_free((char *)bp, bp->mempool);
}

fract32 BPF_set_freq(BPF *const bpf, fract32 hp_freq, fract32 lp_freq) {

    t_BPF *bp = *bpf;

    HPF_set_freq(&bp->hp, hp_freq);
    LPF_set_freq(&bp->lp, lp_freq);
}

fract32 BPF_next(BPF *const bpf, fract32 in) {

    t_BPF *bp = *bpf;

    return LPF_next(&(bp->lp), HPF_next(&(bp->hp), in));
}

fract32 BPF_next_precise(BPF *const bpf, fract32 in) {

    t_BPF *bp = *bpf;

    return LPF_next_precise(&(bp->lp), HPF_next_precise(&(bp->hp), in));
}

fract32 BPF_next_dynamic(BPF *const bpf, fract32 in, fract32 hp_freq,
                         fract32 lp_freq) {
    t_BPF *bp = *bpf;

    return LPF_next_dynamic(&(bp->lp), HPF_next_dynamic(&(bp->hp), in, hp_freq),
                            lp_freq);
}

fract32 BPF_next_dynamic_precise(BPF *const bpf, fract32 in, fract32 hp_freq,
                                 fract32 lp_freq) {
    t_BPF *bp = *bpf;

    return LPF_next_dynamic_precise(
        &(bp->lp), HPF_next_dynamic_precise(&(bp->hp), in, hp_freq), lp_freq);
}

fract32 HPF_dc_block(HPF *const hpf, fract32 in) {

    t_HPF *hp = *hpf;

    fract32 in_scaled = shr_fr1x32(in, 3);
    fract32 aux = sub_fr1x32(in_scaled, hp->lastIn);

    hp->lastOut = add_fr1x32(aux, mult_fr1x32x32(0x7F600000, hp->lastOut));
    hp->lastIn = in_scaled;

    return shl_fr1x32(hp->lastOut, 3);
}

fract32 HPF_dc_block2(HPF *const hpf, fract32 in) {

    t_HPF *hp = *hpf;

    fract32 in_scaled = shr_fr1x32(in, 3);

    hp->lastOut = mult_fr1x32x32(
        sub_fr1x32(add_fr1x32(in_scaled, hp->lastOut), hp->lastIn), 0x7F600000);

    hp->lastIn = in_scaled;

    return shl_fr1x32(hp->lastOut, 3);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
