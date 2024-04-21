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
 * @file    filter_svf.c
 *
 * @brief   A digital state-variable filter for 32-bit fixed point audio.
 *          2x oversampling for better frequency range.
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph-mempool.h"
#include "aleph.h"

#include "filter_svf.h"
#include "soft_clip.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

static void _calc_frame(t_FilterSVF *f, fract32 in);
static void _softclip_calc_frame(t_FilterSVF *f, fract32 in);
static void _softclip_asym_calc_frame(t_FilterSVF *f, fract32 in);

/*----- Extern function implementations ------------------------------*/

void FilterSVF_init(t_FilterSVF *f, Aleph *const aleph) {

    FilterSVF_init_to_pool(f, aleph->mempool);
}

void FilterSVF_init_to_pool(t_FilterSVF *f, t_Mempool *mp) {

    f = (t_FilterSVF *)mpool_alloc(sizeof(t_FilterSVF), mp);

    f->freq = 0;
    f->low = f->high = f->band = f->notch = 0;
    f->lowMix = f->highMix = f->bandMix = f->notchMix = f->peakMix = 0;
}

// Set reciprocal of Q.
void FilterSVF_set_rq(t_FilterSVF *f, fract32 rq) {
    // rq range is [0, 2],
    // fract32 positive range is [0, .9999...]
    // so: move the radix to interpret rq as 2.0

    f->rqShift = norm_fr1x32(rq);
    f->rq = shl_fr1x32(rq, f->rqShift);
}

// Set cutoff coefficient directly.
void FilterSVF_set_coeff(t_FilterSVF *f, fract32 coeff) { f->freq = coeff; }

// Set output mixes.
void FilterSVF_set_low(t_FilterSVF *f, fract32 mix) { f->lowMix = mix; }

void FilterSVF_set_high(t_FilterSVF *f, fract32 mix) { f->highMix = mix; }

void FilterSVF_set_band(t_FilterSVF *f, fract32 mix) { f->bandMix = mix; }

void FilterSVF_set_notch(t_FilterSVF *f, fract32 mix) { f->notchMix = mix; }

void FilterSVF_set_peak(t_FilterSVF *f, fract32 mix) { f->peakMix = mix; }

fract32 FilterSVF_mix_outputs(t_FilterSVF *f) {

    f->notch = add_fr1x32(f->low, f->high);

    fract32 out = mult_fr1x32x32(f->low, f->lowMix);
    out = add_fr1x32(out, mult_fr1x32x32(f->low, f->lowMix));
    out = add_fr1x32(out, mult_fr1x32x32(f->high, f->highMix));
    out = add_fr1x32(out, mult_fr1x32x32(f->band, f->bandMix));
    out = add_fr1x32(out, mult_fr1x32x32(f->notch, f->notchMix));
    return out;
}

// Get next value (with input).
fract32 FilterSVF_next(t_FilterSVF *f, fract32 in) {
    // process 2x and average
    _calc_frame(f, in);
    fract32 out = shr_fr1x32(FilterSVF_mix_outputs(f), 1);
    _calc_frame(f, in);
    out = add_fr1x32(out, shr_fr1x32(FilterSVF_mix_outputs(f), 1));
    return out;
}

fract32 FilterSVF_lpf_next(t_FilterSVF *f, fract32 in) {
    _calc_frame(f, in);
    return f->low;
}

fract32 FilterSVF_bpf_next(t_FilterSVF *f, fract32 in) {
    _calc_frame(f, in);
    return f->band;
}

fract32 FilterSVF_hpf_next(t_FilterSVF *f, fract32 in) {
    _calc_frame(f, in);
    return f->high;
}

fract32 FilterSVF_notch_next(t_FilterSVF *f, fract32 in) {
    _calc_frame(f, in);
    return add_fr1x32(f->low, f->high);
}

fract32 FilterSVF_softclip_lpf_next(t_FilterSVF *f, fract32 in) {
    _softclip_calc_frame(f, in);
    return f->low;
}

fract32 FilterSVF_softclip_bpf_next(t_FilterSVF *f, fract32 in) {
    _softclip_calc_frame(f, in);
    return f->band;
}

fract32 FilterSVF_softclip_notch_next(t_FilterSVF *f, fract32 in) {
    _softclip_calc_frame(f, in);
    return add_fr1x32(f->low, f->high);
}

fract32 FilterSVF_softclip_hpf_next(t_FilterSVF *f, fract32 in) {
    _softclip_calc_frame(f, in);
    return f->high;
}

fract32 FilterSVF_softclip_asym_lpf_next(t_FilterSVF *f, fract32 in) {
    _softclip_asym_calc_frame(f, in);
    return f->low;
}

fract32 FilterSVF_softclip_asym_bpf_next(t_FilterSVF *f, fract32 in) {
    _softclip_asym_calc_frame(f, in);
    return f->band;
}

fract32 FilterSVF_softclip_asym_hpf_next(t_FilterSVF *f, fract32 in) {
    _softclip_asym_calc_frame(f, in);
    return f->high;
}

fract32 FilterSVF_softclip_asym_notch_next(t_FilterSVF *f, fract32 in) {
    _softclip_asym_calc_frame(f, in);
    return add_fr1x32(f->low, f->high);
}

const p_svf_func FilterSVF_func[3][4] = {
    {FilterSVF_hpf_next, FilterSVF_bpf_next, FilterSVF_lpf_next,
     FilterSVF_notch_next},
    {FilterSVF_softclip_hpf_next, FilterSVF_softclip_bpf_next,
     FilterSVF_softclip_lpf_next, FilterSVF_softclip_notch_next},
    {FilterSVF_softclip_asym_hpf_next, FilterSVF_softclip_asym_bpf_next,
     FilterSVF_softclip_asym_lpf_next, FilterSVF_softclip_asym_notch_next}};

/*----- Static function implementations ------------------------------*/

static void _FilterSVF_calc_frame(t_FilterSVF *f, fract32 in) {
    f->low = add_fr1x32(f->low, mult_fr1x32x32(f->freq, f->band));

    f->high = sub_fr1x32(
        sub_fr1x32(in, shr_fr1x32(mult_fr1x32x32(f->rq, f->band), f->rqShift)),
        f->low);

    f->band = add_fr1x32(f->band, mult_fr1x32x32(f->freq, f->high));
}

static void _FilterSVF_softclip_calc_frame(t_FilterSVF *f, fract32 in) {
    char clip_radix = 0;
    f->low = shr_fr1x32(
        soft_clip(
            FR32_MAX / 2,
            shl_fr1x32(add_fr1x32(f->low, mult_fr1x32x32(f->freq, f->band)),
                       clip_radix)),
        clip_radix);
    f->high = shr_fr1x32(
        soft_clip(
            FR32_MAX / 2,
            shl_fr1x32(sub_fr1x32(sub_fr1x32(in, shr_fr1x32(mult_fr1x32x32(
                                                                f->rq, f->band),
                                                            f->rqShift)),
                                  f->low),
                       clip_radix)),
        clip_radix);
    f->band = shr_fr1x32(
        soft_clip(
            FR32_MAX / 2,
            shl_fr1x32(add_fr1x32(f->band, mult_fr1x32x32(f->freq, f->high)),
                       clip_radix)),
        clip_radix);
}

static void _FilterSVF_softclip_asym_calc_frame(t_FilterSVF *f, fract32 in) {
    char clip_radix = 0;

    f->low = shr_fr1x32(
        soft_clip(
            FR32_MAX / 4,
            shl_fr1x32(add_fr1x32(f->low, mult_fr1x32x32(f->freq, f->band)),
                       clip_radix)),
        clip_radix);
    f->high = shr_fr1x32(
        soft_clip(
            FR32_MAX / 8,
            shl_fr1x32(sub_fr1x32(sub_fr1x32(in, shr_fr1x32(mult_fr1x32x32(
                                                                f->rq, f->band),
                                                            f->rqShift)),
                                  f->low),
                       clip_radix)),
        clip_radix);
    f->band = shr_fr1x32(
        soft_clip_asym(
            FR32_MAX / 4, FR32_MAX / -8,
            shl_fr1x32(add_fr1x32(f->band, mult_fr1x32x32(f->freq, f->high)),
                       clip_radix)),
        clip_radix);
}

/*----- End of file --------------------------------------------------*/
