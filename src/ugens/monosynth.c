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
 * @file    monosynth.c
 *
 * @brief   Monosynth module.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "lpf_one_pole.h"
#include "ugens/env_adsr.h"
#include "ugens/filter.h"
#include "ugens/filter_svf.h"
#include "ugens/oscillator.h"
#include "ugens/waveform.h"

#include "ugens/monosynth.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/
void MonoSynth_init(MonoSynth *const synth, t_Aleph *aleph) {

    MonoSynth_init_to_pool(synth, &aleph->mempool);
}

void MonoSynth_init_to_pool(MonoSynth *const synth, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_MonoSynth *syn = *synth =
        (t_MonoSynth *)mpool_alloc(sizeof(t_MonoSynth), mp);

    syn->mempool = mp;

    syn->freq = MONOSYNTH_DEFAULT_FREQ;
    syn->freq_offset = MONOSYNTH_DEFAULT_FREQ_OFFSET;
    syn->cutoff = MONOSYNTH_DEFAULT_CUTOFF;
    syn->res = MONOSYNTH_DEFAULT_RES;
    syn->filter_type = MONOSYNTH_DEFAULT_FILTER_TYPE;

    syn->amp_env_depth = MONOSYNTH_DEFAULT_AMP_ENV_DEPTH;
    syn->filter_env_depth = MONOSYNTH_DEFAULT_FILTER_ENV_DEPTH;
    syn->pitch_env_depth = MONOSYNTH_DEFAULT_PITCH_ENV_DEPTH;

    syn->amp_lfo_depth = MONOSYNTH_DEFAULT_AMP_LFO_DEPTH;
    syn->filter_lfo_depth = MONOSYNTH_DEFAULT_FILTER_LFO_DEPTH;
    syn->pitch_lfo_depth = MONOSYNTH_DEFAULT_PITCH_LFO_DEPTH;

    WaveformDual_init_to_pool(&syn->waveform, mempool);

    FilterSVF_init_to_pool(&syn->filter, mempool);

    HPF_init_to_pool(&syn->dc_block, mempool);

    EnvADSR_init_to_pool(&syn->amp_env, mempool);
    EnvADSR_init_to_pool(&syn->filter_env, mempool);
    EnvADSR_init_to_pool(&syn->pitch_env, mempool);

    Oscillator_init_to_pool(&syn->amp_lfo, mempool);
    Oscillator_init_to_pool(&syn->filter_lfo, mempool);
    Oscillator_init_to_pool(&syn->pitch_lfo, mempool);

    LPFOnePole_init_to_pool(&syn->freq_slew, syn->freq, mempool);
    LPFOnePole_init_to_pool(&syn->freq_offset_slew, syn->freq_offset, mempool);
    LPFOnePole_init_to_pool(&syn->cutoff_slew, syn->cutoff, mempool);
    LPFOnePole_init_to_pool(&syn->res_slew, syn->res, mempool);
}

void MonoSynth_free(MonoSynth *const synth) {

    t_MonoSynth *syn = *synth;

    WaveformDual_free(&syn->waveform);

    FilterSVF_free(&syn->filter);

    HPF_free(&syn->dc_block);

    EnvADSR_free(&syn->amp_env);
    EnvADSR_free(&syn->filter_env);
    EnvADSR_free(&syn->pitch_env);

    Oscillator_free(&syn->amp_lfo);
    Oscillator_free(&syn->filter_lfo);
    Oscillator_free(&syn->pitch_lfo);

    LPFOnePole_free(&syn->freq_slew);
    LPFOnePole_free(&syn->freq_offset_slew);
    LPFOnePole_free(&syn->cutoff_slew);
    LPFOnePole_free(&syn->res_slew);

    mpool_free((char *)syn, syn->mempool);
}

fract32 MonoSynth_next(MonoSynth *const synth) {

    t_MonoSynth *syn = *synth;

    fract32 waveform;

    fract32 freq;
    fract32 freq_offset;

    fract32 amp_lfo;
    fract32 filter_lfo;
    fract32 pitch_lfo;

    fract32 amp_env;
    fract32 filter_env;
    fract32 pitch_env;

    fract32 cutoff;
    fract32 res;

    // Calculate pitch LFO.
    pitch_lfo = Oscillator_next(&syn->pitch_lfo);

    // Scale pitch LFO depth.
    pitch_lfo = mult_fr1x32x32(pitch_lfo, syn->pitch_lfo_depth);

    // Calculate pitch envelope.
    pitch_env = EnvADSR_next(&syn->pitch_env);

    // Scale pitch envelope depth.
    pitch_env = mult_fr1x32x32(pitch_env, syn->pitch_env_depth);

    // Calculate amp LFO.
    amp_lfo = Oscillator_next(&syn->amp_lfo);

    // Scale amp LFO depth.
    amp_lfo = mult_fr1x32x32(amp_lfo, syn->amp_lfo_depth);

    // Calculate amplitude envelope.
    amp_env = EnvADSR_next(&syn->amp_env);

    // Scale amplitude envelope.
    amp_env = mult_fr1x32x32(amp_env, syn->amp_env_depth);

    // Calculate filter LFO.
    filter_lfo = Oscillator_next(&syn->filter_lfo);

    // Scale filter LFO depth.
    filter_lfo = mult_fr1x32x32(filter_lfo, syn->filter_lfo_depth);

    // Calculate filter envelope.
    filter_env = EnvADSR_next(&syn->filter_env);

    // Scale filter envelope depth.
    filter_env = mult_fr1x32x32(filter_env, syn->filter_env_depth);

    // Get slewed frequency.
    freq = LPFOnePole_next(&syn->freq_slew);

    // Get slewed frequency offset.
    freq_offset = LPFOnePole_next(&syn->freq_offset_slew);

    // Apply pitch envelope.
    freq = add_fr1x32(pitch_env, freq);

    // Apply pitch LFO.
    freq = add_fr1x32(freq, mult_fr1x32x32(freq, pitch_lfo));

    // Set oscillator frequency.
    WaveformDual_set_freq(&syn->waveform, freq,
                          fix16_mul_fract(freq, freq_offset));

    // Generate waveforms.
    waveform = WaveformDual_next(&syn->waveform);

    // Shift right to prevent clipping.
    waveform = shr_fr1x32(waveform, 1);

    // Apply amp envelope.
    waveform = mult_fr1x32x32(waveform, amp_env);

    // Apply amp LFO.
    waveform = add_fr1x32(waveform, mult_fr1x32x32(waveform, amp_lfo));

    // Get slewed cutoff.
    cutoff = LPFOnePole_next(&syn->cutoff_slew);

    // Get slewed resonance.
    res = LPFOnePole_next(&syn->res_slew);

    // Apply filter envelope.
    cutoff = add_fr1x32(filter_env, cutoff);

    // Apply filter LFO.
    cutoff = add_fr1x32(cutoff, mult_fr1x32x32(cutoff, filter_lfo));

    // Set filter cutoff and resonance.
    FilterSVF_set_coeff(&syn->filter, cutoff);
    FilterSVF_set_rq(&syn->filter, res);

    // Apply filter.
    switch (syn->filter_type) {

    case FILTERSVF_TYPE_LPF:
        waveform = FilterSVF_softclip_asym_lpf_next(&syn->filter, waveform);
        break;

    case FILTERSVF_TYPE_BPF:
        waveform = FilterSVF_softclip_asym_bpf_next(&syn->filter, waveform);
        break;

    case FILTERSVF_TYPE_HPF:
        waveform = FilterSVF_softclip_asym_hpf_next(&syn->filter, waveform);
        break;

    default:
        // Default to LPF.
        waveform = FilterSVF_softclip_asym_lpf_next(&syn->filter, waveform);
        break;
    }

    // Block DC.
    waveform = HPF_dc_block(&syn->dc_block, waveform);

    return waveform;
}

void MonoSynth_set_shape(MonoSynth *const synth, e_Waveform_shape shape_a,
                         e_Waveform_shape shape_b) {

    t_MonoSynth *syn = *synth;

    WaveformDual_set_shape(&syn->waveform, shape_a, shape_b);
}

void MonoSynth_set_freq(MonoSynth *const synth, fract32 freq) {

    t_MonoSynth *syn = *synth;

    LPFOnePole_set_input(&syn->freq_slew, freq);
}

void MonoSynth_set_freq_offset(MonoSynth *const synth, fract32 freq_offset) {

    t_MonoSynth *syn = *synth;

    LPFOnePole_set_input(&syn->freq_offset_slew, freq_offset);
}

void MonoSynth_set_cutoff(MonoSynth *const synth, fract32 cutoff) {

    t_MonoSynth *syn = *synth;

    LPFOnePole_set_input(&syn->cutoff_slew, cutoff);
}

void MonoSynth_set_res(MonoSynth *const synth, fract32 res) {

    t_MonoSynth *syn = *synth;

    LPFOnePole_set_input(&syn->res_slew, res);
}

void MonoSynth_set_amp_env_attack(MonoSynth *const synth, fract32 attack) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_attack(&syn->amp_env, attack);
}

void MonoSynth_set_amp_env_decay(MonoSynth *const synth, fract32 decay) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_decay(&syn->amp_env, decay);
}

void MonoSynth_set_amp_env_sustain(MonoSynth *const synth, fract32 sustain) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_sustain(&syn->amp_env, sustain);
}

void MonoSynth_set_amp_env_release(MonoSynth *const synth, fract32 release) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_release(&syn->amp_env, release);
}

void MonoSynth_set_filter_env_attack(MonoSynth *const synth, fract32 attack) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_attack(&syn->filter_env, attack);
}

void MonoSynth_set_filter_env_decay(MonoSynth *const synth, fract32 decay) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_decay(&syn->filter_env, decay);
}

void MonoSynth_set_filter_env_sustain(MonoSynth *const synth, fract32 sustain) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_sustain(&syn->filter_env, sustain);
}

void MonoSynth_set_filter_env_release(MonoSynth *const synth, fract32 release) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_release(&syn->filter_env, release);
}

void MonoSynth_set_pitch_env_attack(MonoSynth *const synth, fract32 attack) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_attack(&syn->pitch_env, attack);
}

void MonoSynth_set_pitch_env_decay(MonoSynth *const synth, fract32 decay) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_decay(&syn->pitch_env, decay);
}

void MonoSynth_set_pitch_env_sustain(MonoSynth *const synth, fract32 sustain) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_sustain(&syn->pitch_env, sustain);
}

void MonoSynth_set_pitch_env_release(MonoSynth *const synth, fract32 release) {

    t_MonoSynth *syn = *synth;

    EnvADSR_set_release(&syn->pitch_env, release);
}

void MonoSynth_set_amp_lfo_freq(MonoSynth *const synth, fract32 freq) {

    t_MonoSynth *syn = *synth;

    Oscillator_set_freq(&syn->amp_lfo, freq);
}

void MonoSynth_set_amp_lfo_depth(MonoSynth *const synth, fract32 depth) {

    t_MonoSynth *syn = *synth;

    syn->amp_lfo_depth = depth;
}

void MonoSynth_set_filter_lfo_freq(MonoSynth *const synth, fract32 freq) {

    t_MonoSynth *syn = *synth;

    Oscillator_set_freq(&syn->filter_lfo, freq);
}

void MonoSynth_set_filter_lfo_depth(MonoSynth *const synth, fract32 depth) {

    t_MonoSynth *syn = *synth;

    syn->filter_lfo_depth = depth;
}

void MonoSynth_set_pitch_lfo_freq(MonoSynth *const synth, fract32 freq) {

    t_MonoSynth *syn = *synth;

    Oscillator_set_freq(&syn->pitch_lfo, freq);
}

void MonoSynth_set_pitch_lfo_depth(MonoSynth *const synth, fract32 depth) {

    t_MonoSynth *syn = *synth;

    syn->pitch_lfo_depth = depth;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
