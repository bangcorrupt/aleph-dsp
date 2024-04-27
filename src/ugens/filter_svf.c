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

#include "ugens/filter_svf.h"
#include "ugens/soft_clip.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

static void _calc_frame(FilterSVF *const filter, fract32 in);
static void _softclip_calc_frame(FilterSVF *const filter, fract32 in);
static void _softclip_asym_calc_frame(FilterSVF *const filter, fract32 in);

/*----- Extern function implementations ------------------------------*/

void FilterSVF_init(FilterSVF *const filter, t_Aleph *const aleph) {

    FilterSVF_init_to_pool(filter, &aleph->mempool);
}

void FilterSVF_init_to_pool(FilterSVF *const filter, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_FilterSVF *fl = *filter =
        (t_FilterSVF *)mpool_alloc(sizeof(t_FilterSVF), mp);

    fl->mempool = mp;

    fl->freq = 0;
    fl->low = fl->high = fl->band = fl->notch = 0;
    fl->low_mix = fl->high_mix = fl->band_mix = fl->notch_mix = fl->peak_mix =
        0;
}

void FilterSVF_free(FilterSVF *const filter) {

    t_FilterSVF *fl = *filter;

    mpool_free((char *)fl, fl->mempool);
}

// Set reciprocal of Q.
void FilterSVF_set_rq(FilterSVF *const filter, fract32 rq) {

    t_FilterSVF *fl = *filter;

    // rq range is [0, 2],
    // fract32 positive range is [0, .9999...]
    // so: move the radix to interpret rq as 2.0

    fl->rq_shift = norm_fr1x32(rq);
    fl->rq = shl_fr1x32(rq, fl->rq_shift);
}

// Set cutoff coefficient directly.
void FilterSVF_set_coeff(FilterSVF *const filter, fract32 coeff) {

    t_FilterSVF *fl = *filter;

    fl->freq = coeff;
}

// Set output mixes.
void FilterSVF_set_low(FilterSVF *const filter, fract32 mix) {

    t_FilterSVF *fl = *filter;

    fl->low_mix = mix;
}

void FilterSVF_set_high(FilterSVF *const filter, fract32 mix) {

    t_FilterSVF *fl = *filter;

    fl->high_mix = mix;
}

void FilterSVF_set_band(FilterSVF *const filter, fract32 mix) {

    t_FilterSVF *fl = *filter;

    fl->band_mix = mix;
}

void FilterSVF_set_notch(FilterSVF *const filter, fract32 mix) {

    t_FilterSVF *fl = *filter;

    fl->notch_mix = mix;
}

void FilterSVF_set_peak(FilterSVF *const filter, fract32 mix) {

    t_FilterSVF *fl = *filter;

    fl->peak_mix = mix;
}

fract32 FilterSVF_mix_outputs(FilterSVF *const filter) {

    t_FilterSVF *fl = *filter;

    fl->notch = add_fr1x32(fl->low, fl->high);

    fract32 out = mult_fr1x32x32(fl->low, fl->low_mix);
    out = add_fr1x32(out, mult_fr1x32x32(fl->low, fl->low_mix));
    out = add_fr1x32(out, mult_fr1x32x32(fl->high, fl->high_mix));
    out = add_fr1x32(out, mult_fr1x32x32(fl->band, fl->band_mix));
    out = add_fr1x32(out, mult_fr1x32x32(fl->notch, fl->notch_mix));
    return out;
}

// Get next value (with input).
fract32 FilterSVF_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    // process 2x and average
    _calc_frame(&fl, in);
    fract32 out = shr_fr1x32(FilterSVF_mix_outputs(&fl), 1);
    _calc_frame(&fl, in);
    out = add_fr1x32(out, shr_fr1x32(FilterSVF_mix_outputs(&fl), 1));
    return out;
}

fract32 FilterSVF_lpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);
    return fl->low;
}

fract32 FilterSVF_bpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);
    return fl->band;
}

fract32 FilterSVF_hpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);
    return fl->high;
}

fract32 FilterSVF_notch_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);
    return add_fr1x32(fl->low, fl->high);
}

fract32 FilterSVF_softclip_lpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _softclip_calc_frame(&fl, in);
    return fl->low;
}

fract32 FilterSVF_softclip_bpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _softclip_calc_frame(&fl, in);
    return fl->band;
}

fract32 FilterSVF_softclip_notch_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _softclip_calc_frame(&fl, in);
    return add_fr1x32(fl->low, fl->high);
}

fract32 FilterSVF_softclip_hpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _softclip_calc_frame(&fl, in);
    return fl->high;
}

fract32 FilterSVF_softclip_asym_lpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _softclip_asym_calc_frame(&fl, in);
    return fl->low;
}

fract32 FilterSVF_softclip_asym_bpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _softclip_asym_calc_frame(&fl, in);
    return fl->band;
}

fract32 FilterSVF_softclip_asym_hpf_next(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    _softclip_asym_calc_frame(&fl, in);
    return fl->high;
}

fract32 FilterSVF_softclip_asym_notch_next(FilterSVF *const filter,
                                           fract32 in) {

    t_FilterSVF *fl = *filter;

    _softclip_asym_calc_frame(&fl, in);
    return add_fr1x32(fl->low, fl->high);
}

const p_svf_func FilterSVF_func[3][4] = {
    {FilterSVF_hpf_next, FilterSVF_bpf_next, FilterSVF_lpf_next,
     FilterSVF_notch_next},
    {FilterSVF_softclip_hpf_next, FilterSVF_softclip_bpf_next,
     FilterSVF_softclip_lpf_next, FilterSVF_softclip_notch_next},
    {FilterSVF_softclip_asym_hpf_next, FilterSVF_softclip_asym_bpf_next,
     FilterSVF_softclip_asym_lpf_next, FilterSVF_softclip_asym_notch_next}};

/*----- Static function implementations ------------------------------*/

static void _calc_frame(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    fl->low = add_fr1x32(fl->low, mult_fr1x32x32(fl->freq, fl->band));

    fl->high =
        sub_fr1x32(sub_fr1x32(in, shr_fr1x32(mult_fr1x32x32(fl->rq, fl->band),
                                             fl->rq_shift)),
                   fl->low);

    fl->band = add_fr1x32(fl->band, mult_fr1x32x32(fl->freq, fl->high));
}

static void _softclip_calc_frame(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    char clip_radix = 0;
    fl->low = shr_fr1x32(
        soft_clip(
            FR32_MAX / 2,
            shl_fr1x32(add_fr1x32(fl->low, mult_fr1x32x32(fl->freq, fl->band)),
                       clip_radix)),
        clip_radix);
    fl->high = shr_fr1x32(
        soft_clip(
            FR32_MAX / 2,
            shl_fr1x32(
                sub_fr1x32(
                    sub_fr1x32(in, shr_fr1x32(mult_fr1x32x32(fl->rq, fl->band),
                                              fl->rq_shift)),
                    fl->low),
                clip_radix)),
        clip_radix);
    fl->band = shr_fr1x32(
        soft_clip(
            FR32_MAX / 2,
            shl_fr1x32(add_fr1x32(fl->band, mult_fr1x32x32(fl->freq, fl->high)),
                       clip_radix)),
        clip_radix);
}

static void _softclip_asym_calc_frame(FilterSVF *const filter, fract32 in) {

    t_FilterSVF *fl = *filter;

    char clip_radix = 0;

    fl->low = shr_fr1x32(
        soft_clip(
            FR32_MAX / 4,
            shl_fr1x32(add_fr1x32(fl->low, mult_fr1x32x32(fl->freq, fl->band)),
                       clip_radix)),
        clip_radix);
    fl->high = shr_fr1x32(
        soft_clip(
            FR32_MAX / 8,
            shl_fr1x32(
                sub_fr1x32(
                    sub_fr1x32(in, shr_fr1x32(mult_fr1x32x32(fl->rq, fl->band),
                                              fl->rq_shift)),
                    fl->low),
                clip_radix)),
        clip_radix);
    fl->band = shr_fr1x32(
        soft_clip_asym(
            FR32_MAX / 4, FR32_MAX / -8,
            shl_fr1x32(add_fr1x32(fl->band, mult_fr1x32x32(fl->freq, fl->high)),
                       clip_radix)),
        clip_radix);
}

/*----- End of file --------------------------------------------------*/
