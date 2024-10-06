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
 * @file    aleph_filter.c
 *
 * @brief   Filters.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_interpolate.h"

#include "aleph_filter.h"

/*----- Macros -------------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_HPF_init(Aleph_HPF *const hpf, t_Aleph *const aleph) {

    Aleph_HPF_init_to_pool(hpf, &aleph->mempool);
}

void Aleph_HPF_init_to_pool(Aleph_HPF *const hpf, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_HPF *hp = *hpf =
        (t_Aleph_HPF *)mpool_alloc(sizeof(t_Aleph_HPF), mp);

    hp->mempool = mp;

    hp->freq = ALEPH_HPF_DEFAULT_FREQ;
    hp->last_in = 0;
    hp->last_out = 0;
}

void Aleph_HPF_free(Aleph_HPF *const hpf) {

    t_Aleph_HPF *hp = *hpf;

    mpool_free((char *)hp, hp->mempool);
}

void Aleph_HPF_set_freq(Aleph_HPF *const hpf, fract32 freq) {

    t_Aleph_HPF *hp = *hpf;

    hp->freq = freq;
}

fract32 Aleph_HPF_next(Aleph_HPF *const hpf, fract32 in) {

    t_Aleph_HPF *hp = *hpf;

    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = hp->freq * 4;

    hp->last_out =
        add_fr1x32(mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), hp->last_out),
                   mult_fr1x32x32(alpha, sub_fr1x32(in, hp->last_in)));

    hp->last_in = in;

    return hp->last_out * (FR32_MAX / hp->freq / 4);
}

fract32 Aleph_HPF_next_precise(Aleph_HPF *const hpf, fract32 in) {

    t_Aleph_HPF *hp = *hpf;

    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = _hpf_freq_calc(hp->freq);

    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, hp->last_out),
                   mult_fr1x32x32(alpha, (sub_fr1x32(in, hp->last_in))));

    hp->last_out = out;
    hp->last_in = in;

    return out;
}

fract32 Aleph_HPF_next_dynamic(Aleph_HPF *const hpf, fract32 in, fract32 freq) {

    t_Aleph_HPF *hp = *hpf;

    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = freq * 4;

    hp->last_out =
        add_fr1x32(mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), hp->last_out),
                   mult_fr1x32x32(alpha, sub_fr1x32(in, hp->last_in)));

    hp->last_in = in;

    return hp->last_out * (FR32_MAX / freq / 4);
}

fract32 Aleph_HPF_next_dynamic_precise(Aleph_HPF *const hpf, fract32 in,
                                       fract32 freq) {

    t_Aleph_HPF *hp = *hpf;

    // Should be 1 / (2 pi dt fc + 1)
    fract32 alpha = _hpf_freq_calc(freq);

    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, hp->last_out),
                   mult_fr1x32x32(alpha, (sub_fr1x32(in, hp->last_in))));

    hp->last_out = out;
    hp->last_in = in;

    return out;
}

void Aleph_LPF_init(Aleph_LPF *const lpf, t_Aleph *const aleph) {

    Aleph_LPF_init_to_pool(lpf, &aleph->mempool);
}

void Aleph_LPF_init_to_pool(Aleph_LPF *const lpf, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_LPF *lp = *lpf =
        (t_Aleph_LPF *)mpool_alloc(sizeof(t_Aleph_LPF), mp);

    lp->mempool = mp;

    lp->freq = ALEPH_LPF_DEFAULT_FREQ;
    lp->last_out = 0;
}

void Aleph_LPF_free(Aleph_LPF *const lpf) {

    t_Aleph_LPF *lp = *lpf;

    mpool_free((char *)lp, lp->mempool);
}

void Aleph_LPF_set_freq(Aleph_LPF *const lpf, fract32 freq) {

    t_Aleph_LPF *lp = *lpf;

    lp->freq = freq;
}

// the frequency unit is fraction of samplerate
fract32 Aleph_LPF_next(Aleph_LPF *const lpf, fract32 in) {

    t_Aleph_LPF *lp = *lpf;

    return SIMPLE_SLEW(lp->last_out, in, TWOPI * lp->freq);
}

fract32 Aleph_LPF_next_precise(Aleph_LPF *const lpf, fract32 in) {

    t_Aleph_LPF *lp = *lpf;

    fract32 alpha = _lpf_freq_calc(lp->freq);

    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, in),
                   mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), lp->last_out));

    lp->last_out = out;

    return out;
}

// the frequency unit is fraction of samplerate
fract32 Aleph_LPF_next_dynamic(Aleph_LPF *const lpf, fract32 in, fract32 freq) {

    t_Aleph_LPF *lp = *lpf;

    return SIMPLE_SLEW(lp->last_out, in, TWOPI * freq);
}

fract32 Aleph_LPF_next_dynamic_precise(Aleph_LPF *const lpf, fract32 in,
                                       fract32 freq) {

    t_Aleph_LPF *lp = *lpf;

    fract32 alpha = _lpf_freq_calc(freq);

    fract32 out =
        add_fr1x32(mult_fr1x32x32(alpha, in),
                   mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), lp->last_out));

    lp->last_out = out;

    return out;
}

void Aleph_BPF_init(Aleph_BPF *const bpf, t_Aleph *const aleph) {

    Aleph_BPF_init_to_pool(bpf, &aleph->mempool);
}

void Aleph_BPF_init_to_pool(Aleph_BPF *const bpf, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_BPF *bp = *bpf =
        (t_Aleph_BPF *)mpool_alloc(sizeof(t_Aleph_BPF), mp);

    bp->mempool = mp;

    Aleph_HPF_init_to_pool(&bp->hp, mempool);
    Aleph_LPF_init_to_pool(&bp->lp, mempool);
}

void Aleph_BPF_free(Aleph_BPF *const bpf) {

    t_Aleph_BPF *bp = *bpf;

    Aleph_HPF_free(&bp->hp);
    Aleph_LPF_free(&bp->lp);

    mpool_free((char *)bp, bp->mempool);
}

void Aleph_BPF_set_freq(Aleph_BPF *const bpf, fract32 hp_freq,
                        fract32 lp_freq) {

    t_Aleph_BPF *bp = *bpf;

    Aleph_HPF_set_freq(&bp->hp, hp_freq);
    Aleph_LPF_set_freq(&bp->lp, lp_freq);
}

fract32 Aleph_BPF_next(Aleph_BPF *const bpf, fract32 in) {

    t_Aleph_BPF *bp = *bpf;

    return Aleph_LPF_next(&(bp->lp), Aleph_HPF_next(&(bp->hp), in));
}

fract32 Aleph_BPF_next_precise(Aleph_BPF *const bpf, fract32 in) {

    t_Aleph_BPF *bp = *bpf;

    return Aleph_LPF_next_precise(&(bp->lp),
                                  Aleph_HPF_next_precise(&(bp->hp), in));
}

fract32 Aleph_BPF_next_dynamic(Aleph_BPF *const bpf, fract32 in,
                               fract32 hp_freq, fract32 lp_freq) {
    t_Aleph_BPF *bp = *bpf;

    return Aleph_LPF_next_dynamic(
        &(bp->lp), Aleph_HPF_next_dynamic(&(bp->hp), in, hp_freq), lp_freq);
}

fract32 Aleph_BPF_next_dynamic_precise(Aleph_BPF *const bpf, fract32 in,
                                       fract32 hp_freq, fract32 lp_freq) {
    t_Aleph_BPF *bp = *bpf;

    return Aleph_LPF_next_dynamic_precise(
        &(bp->lp), Aleph_HPF_next_dynamic_precise(&(bp->hp), in, hp_freq),
        lp_freq);
}

fract32 Aleph_HPF_dc_block(Aleph_HPF *const hpf, fract32 in) {

    t_Aleph_HPF *hp = *hpf;

    fract32 in_scaled = shr_fr1x32(in, 3);
    fract32 aux = sub_fr1x32(in_scaled, hp->last_in);

    hp->last_out = add_fr1x32(aux, mult_fr1x32x32(0x7F600000, hp->last_out));
    hp->last_in = in_scaled;

    return shl_fr1x32(hp->last_out, 3);
}

fract32 Aleph_HPF_dc_block2(Aleph_HPF *const hpf, fract32 in) {

    t_Aleph_HPF *hp = *hpf;

    fract32 in_scaled = shr_fr1x32(in, 3);

    hp->last_out = mult_fr1x32x32(
        sub_fr1x32(add_fr1x32(in_scaled, hp->last_out), hp->last_in),
        0x7F600000);

    hp->last_in = in_scaled;

    return shl_fr1x32(hp->last_out, 3);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
