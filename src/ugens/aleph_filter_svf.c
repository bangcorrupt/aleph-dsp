/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by monome, modified by bangcorrupt 2024. */

/**
 * @file    aleph_filter_svf.c
 *
 * @brief   A digital state-variable filter for 32-bit fixed point audio.
 *          2x oversampling for better frequency range.
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_filter_svf.h"
#include "aleph_soft_clip.h"
#include "fract_math.h"
#include "fract_typedef.h"
#include "types.h"
#include <stddef.h>

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

static void _calc_frame(Aleph_FilterSVF *const filter, fract32 in);
static void _softclip_calc_frame(Aleph_FilterSVF *const filter, fract32 in);
static void _softclip_asym_calc_frame(Aleph_FilterSVF *const filter,
                                      fract32 in);

/*----- Extern function implementations ------------------------------*/

void Aleph_FilterSVF_init(Aleph_FilterSVF *const filter, t_Aleph *const aleph) {

    Aleph_FilterSVF_init_to_pool(filter, &aleph->mempool);
}

void Aleph_FilterSVF_init_to_pool(Aleph_FilterSVF *const filter,
                                  Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_FilterSVF *fl = *filter =
        (t_Aleph_FilterSVF *)mpool_alloc(sizeof(t_Aleph_FilterSVF), mp);

    fl->mempool = mp;

    fl->freq = 0;
    fl->low = fl->high = fl->band = fl->notch = 0;
    fl->low_mix = fl->high_mix = fl->band_mix = fl->notch_mix = fl->peak_mix =
        0;
}

void Aleph_FilterSVF_free(Aleph_FilterSVF *const filter) {

    t_Aleph_FilterSVF *fl = *filter;

    mpool_free((char *)fl, fl->mempool);
}

// Set reciprocal of Q.
void Aleph_FilterSVF_set_rq(Aleph_FilterSVF *const filter, fract32 rq) {

    t_Aleph_FilterSVF *fl = *filter;

    // rq range is [0, 2],
    // fract32 positive range is [0, .9999...]
    // so: move the radix to interpret rq as 2.0

    fl->rq_shift = norm_fr1x32(rq);
    fl->rq = shl_fr1x32(rq, fl->rq_shift);
}

// Set cutoff coefficient directly.
void Aleph_FilterSVF_set_coeff(Aleph_FilterSVF *const filter, fract32 coeff) {

    t_Aleph_FilterSVF *fl = *filter;

    fl->freq = coeff;
}

// Set output mixes.
void Aleph_FilterSVF_set_low(Aleph_FilterSVF *const filter, fract32 mix) {

    t_Aleph_FilterSVF *fl = *filter;

    fl->low_mix = mix;
}

void Aleph_FilterSVF_set_high(Aleph_FilterSVF *const filter, fract32 mix) {

    t_Aleph_FilterSVF *fl = *filter;

    fl->high_mix = mix;
}

void Aleph_FilterSVF_set_band(Aleph_FilterSVF *const filter, fract32 mix) {

    t_Aleph_FilterSVF *fl = *filter;

    fl->band_mix = mix;
}

void Aleph_FilterSVF_set_notch(Aleph_FilterSVF *const filter, fract32 mix) {

    t_Aleph_FilterSVF *fl = *filter;

    fl->notch_mix = mix;
}

void Aleph_FilterSVF_set_peak(Aleph_FilterSVF *const filter, fract32 mix) {

    t_Aleph_FilterSVF *fl = *filter;

    fl->peak_mix = mix;
}

fract32 Aleph_FilterSVF_mix_outputs(Aleph_FilterSVF *const filter) {

    t_Aleph_FilterSVF *fl = *filter;

    fl->notch = add_fr1x32(fl->low, fl->high);

    fract32 out = mult_fr1x32x32(fl->low, fl->low_mix);
    out = add_fr1x32(out, mult_fr1x32x32(fl->low, fl->low_mix));
    out = add_fr1x32(out, mult_fr1x32x32(fl->high, fl->high_mix));
    out = add_fr1x32(out, mult_fr1x32x32(fl->band, fl->band_mix));
    out = add_fr1x32(out, mult_fr1x32x32(fl->notch, fl->notch_mix));
    return out;
}

// Get next value (with input).
fract32 Aleph_FilterSVF_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);

    return Aleph_FilterSVF_mix_outputs(&fl);
}

fract32 Aleph_FilterSVF_os_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 out;

    // process 2x and average
    _calc_frame(&fl, in);
    out = shr_fr1x32(Aleph_FilterSVF_mix_outputs(&fl), 1);

    _calc_frame(&fl, in);
    out = add_fr1x32(out, shr_fr1x32(Aleph_FilterSVF_mix_outputs(&fl), 1));

    return out;
}

fract32 Aleph_FilterSVF_lpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);
    return fl->low;
}

fract32 Aleph_FilterSVF_os_lpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 out;

    _calc_frame(&fl, in);
    out = shl_fr1x32(fl->low, 1);

    _calc_frame(&fl, in);
    out = add_fr1x32(out, shl_fr1x32(fl->low, 1));

    return fl->low;
}

fract32 Aleph_FilterSVF_sc_lpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _softclip_calc_frame(&fl, in);
    return fl->low;
}

fract32 Aleph_FilterSVF_sc_os_lpf_next(Aleph_FilterSVF *const filter,
                                       fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 out;

    _softclip_calc_frame(&fl, in);
    out = shl_fr1x32(fl->low, 1);

    _softclip_calc_frame(&fl, in);
    out = add_fr1x32(out, shl_fr1x32(fl->low, 1));

    /// TODO: Why do We calculate `out` but return fl->low !?
    //
    return fl->low;
}

void Aleph_FilterSVF_sc_os_lpf_next_block(Aleph_FilterSVF *const filter,
                                          fract32 *input, fract32 *output,
                                          size_t size) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 in;

    int i;
    for (i = 0; i < size; i++) {

        // Allows using same buffer for input and output.
        in = input[i];

        _softclip_calc_frame(&fl, in);
        output[i] = shl_fr1x32(fl->low, 1);

        _softclip_calc_frame(&fl, in);
        output[i] = add_fr1x32(output[i], shl_fr1x32(fl->low, 1));
    }
}

void Aleph_FilterSVF_sc_os_lpf_next_block_smooth(Aleph_FilterSVF *const filter,
                                                 fract32 *freq, fract32 *input,
                                                 fract32 *output, size_t size) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 in;

    int i;
    for (i = 0; i < size; i++) {

        // Allows using same buffer for input and output.
        in = input[i];

        fl->freq = freq[i];

        _softclip_calc_frame(&fl, in);
        output[i] = shl_fr1x32(fl->low, 1);

        _softclip_calc_frame(&fl, in);
        output[i] = add_fr1x32(output[i], shl_fr1x32(fl->low, 1));
    }
}

fract32 Aleph_FilterSVF_sc_asym_lpf_next(Aleph_FilterSVF *const filter,
                                         fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _softclip_asym_calc_frame(&fl, in);
    return fl->low;
}

fract32 Aleph_FilterSVF_bpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);
    return fl->band;
}

fract32 Aleph_FilterSVF_os_bpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 out;

    _calc_frame(&fl, in);
    out = shl_fr1x32(fl->band, 1);

    _calc_frame(&fl, in);
    out = add_fr1x32(out, shl_fr1x32(fl->band, 1));

    return fl->band;
}

fract32 Aleph_FilterSVF_sc_bpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _softclip_calc_frame(&fl, in);
    return fl->band;
}

fract32 Aleph_FilterSVF_sc_os_bpf_next(Aleph_FilterSVF *const filter,
                                       fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 out;

    _softclip_calc_frame(&fl, in);
    out = shl_fr1x32(fl->band, 1);

    _softclip_calc_frame(&fl, in);
    out = add_fr1x32(out, shl_fr1x32(fl->band, 1));

    return fl->band;
}

fract32 Aleph_FilterSVF_sc_asym_bpf_next(Aleph_FilterSVF *const filter,
                                         fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _softclip_asym_calc_frame(&fl, in);
    return fl->band;
}

fract32 Aleph_FilterSVF_hpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);
    return fl->high;
}

fract32 Aleph_FilterSVF_os_hpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 out;

    _calc_frame(&fl, in);
    out = shl_fr1x32(fl->high, 1);

    _calc_frame(&fl, in);
    out = add_fr1x32(out, shl_fr1x32(fl->high, 1));

    return fl->high;
}

fract32 Aleph_FilterSVF_sc_hpf_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _softclip_calc_frame(&fl, in);
    return fl->high;
}

fract32 Aleph_FilterSVF_sc_os_hpf_next(Aleph_FilterSVF *const filter,
                                       fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    fract32 out;

    _softclip_calc_frame(&fl, in);
    out = shl_fr1x32(fl->high, 1);

    _softclip_calc_frame(&fl, in);
    out = add_fr1x32(out, shl_fr1x32(fl->high, 1));

    return fl->high;
}

fract32 Aleph_FilterSVF_sc_asym_hpf_next(Aleph_FilterSVF *const filter,
                                         fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _softclip_asym_calc_frame(&fl, in);
    return fl->high;
}

fract32 Aleph_FilterSVF_notch_next(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _calc_frame(&fl, in);
    return add_fr1x32(fl->low, fl->high);
}

fract32 Aleph_FilterSVF_sc_notch_next(Aleph_FilterSVF *const filter,
                                      fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _softclip_calc_frame(&fl, in);
    return add_fr1x32(fl->low, fl->high);
}

fract32 Aleph_FilterSVF_sc_asym_notch_next(Aleph_FilterSVF *const filter,
                                           fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    _softclip_asym_calc_frame(&fl, in);
    return add_fr1x32(fl->low, fl->high);
}

/// TODO: Add array of oversampling functions.

const p_Aleph_FilterSVF_func Aleph_FilterSVF_func[3][4] = {
    {Aleph_FilterSVF_lpf_next, Aleph_FilterSVF_hpf_next,
     Aleph_FilterSVF_bpf_next, Aleph_FilterSVF_notch_next},
    {Aleph_FilterSVF_sc_lpf_next, Aleph_FilterSVF_sc_hpf_next,
     Aleph_FilterSVF_sc_bpf_next, Aleph_FilterSVF_sc_notch_next},
    {Aleph_FilterSVF_sc_asym_lpf_next, Aleph_FilterSVF_sc_asym_hpf_next,
     Aleph_FilterSVF_sc_asym_bpf_next, Aleph_FilterSVF_sc_asym_notch_next}};

/*----- Static function implementations ------------------------------*/

static void _calc_frame(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

    fl->low = add_fr1x32(fl->low, mult_fr1x32x32(fl->freq, fl->band));

    fl->high =
        sub_fr1x32(sub_fr1x32(in, shr_fr1x32(mult_fr1x32x32(fl->rq, fl->band),
                                             fl->rq_shift)),
                   fl->low);

    fl->band = add_fr1x32(fl->band, mult_fr1x32x32(fl->freq, fl->high));
}

static void _softclip_calc_frame(Aleph_FilterSVF *const filter, fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

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

static void _softclip_asym_calc_frame(Aleph_FilterSVF *const filter,
                                      fract32 in) {

    t_Aleph_FilterSVF *fl = *filter;

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
