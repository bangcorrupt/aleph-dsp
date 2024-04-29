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
 * @file    monosynth.h
 *
 * @brief   Public API for monosynth module.
 *
 */

#ifndef ALEPH_MONOSYNTH_H
#define ALEPH_MONOSYNTH_H

#include "lpf_one_pole.h"
#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "ugens/env_adsr.h"
#include "ugens/filter.h"
#include "ugens/filter_svf.h"
#include "ugens/oscillator.h"
#include "ugens/waveform.h"

/*----- Macros and Definitions ---------------------------------------*/

#define MONOSYNTH_DEFAULT_FREQ (220 << 16)
#define MONOSYNTH_DEFAULT_FREQ_OFFSET (0)

#define MONOSYNTH_DEFAULT_CUTOFF (20000 << 16)
#define MONOSYNTH_DEFAULT_RES (FR32_MAX)
#define MONOSYNTH_DEFAULT_FILTER_TYPE FILTERSVF_TYPE_LPF

#define MONOSYNTH_DEFAULT_AMP_ENV_DEPTH (FR32_MAX)
#define MONOSYNTH_DEFAULT_FILTER_ENV_DEPTH (0)
#define MONOSYNTH_DEFAULT_PITCH_ENV_DEPTH (0)

#define MONOSYNTH_DEFAULT_AMP_LFO_DEPTH (0)
#define MONOSYNTH_DEFAULT_FILTER_LFO_DEPTH (0)
#define MONOSYNTH_DEFAULT_PITCH_LFO_DEPTH (0)

#define MONOSYNTH_DEFAULT_PHASE_RESET (true)

typedef struct {

    Mempool mempool;

    WaveformDual waveform;
    fract32 freq;
    fract32 freq_offset;

    FilterSVF filter;
    e_FilterSVF_type filter_type;

    HPF dc_block;

    EnvADSR amp_env;
    EnvADSR pitch_env;
    EnvADSR filter_env;

    fract32 amp_env_depth;
    fract32 filter_env_depth;
    fract32 pitch_env_depth;

    Oscillator amp_lfo;
    Oscillator filter_lfo;
    Oscillator pitch_lfo;

    fract32 amp_lfo_depth;
    fract32 filter_lfo_depth;
    fract32 pitch_lfo_depth;

    LPFOnePole freq_slew;
    LPFOnePole freq_offset_slew;
    LPFOnePole cutoff_slew;
    LPFOnePole res_slew;

    bool phase_reset;

} t_Aleph_MonoSynth;

typedef t_Aleph_MonoSynth *Aleph_MonoSynth;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_MonoSynth_init(Aleph_MonoSynth *const synth, t_Aleph *aleph);
void Aleph_MonoSynth_init_to_pool(Aleph_MonoSynth *const synth,
                                  Mempool *const mempool);
void Aleph_MonoSynth_free(Aleph_MonoSynth *const synth);

fract32 Aleph_MonoSynth_next(Aleph_MonoSynth *const synth);

void Aleph_MonoSynth_set_shape(Aleph_MonoSynth *const synth,
                               e_Waveform_shape shape_a);
void Aleph_MonoSynth_set_freq(Aleph_MonoSynth *const synth, fract32 freq);
void Aleph_MonoSynth_set_freq_offset(Aleph_MonoSynth *const synth,
                                     fract32 freq_offset);

void Aleph_MonoSynth_set_filter_type(Aleph_MonoSynth *const synth,
                                     e_FilterSVF_type type);
void Aleph_MonoSynth_set_cutoff(Aleph_MonoSynth *const synth, fract32 cutoff);
void Aleph_MonoSynth_set_res(Aleph_MonoSynth *const synth, fract32 res);

void Aleph_MonoSynth_set_amp_env_attack(Aleph_MonoSynth *const synth,
                                        fract32 attack);
void Aleph_MonoSynth_set_amp_env_decay(Aleph_MonoSynth *const synth,
                                       fract32 decay);
void Aleph_MonoSynth_set_amp_env_sustain(Aleph_MonoSynth *const synth,
                                         fract32 sustain);
void Aleph_MonoSynth_set_amp_env_release(Aleph_MonoSynth *const synth,
                                         fract32 release);
void Aleph_MonoSynth_set_amp_env_depth(Aleph_MonoSynth *const synth,
                                       fract32 depth);

void Aleph_MonoSynth_set_filter_env_attack(Aleph_MonoSynth *const synth,
                                           fract32 attack);
void Aleph_MonoSynth_set_filter_env_decay(Aleph_MonoSynth *const synth,
                                          fract32 decay);
void Aleph_MonoSynth_set_filter_env_sustain(Aleph_MonoSynth *const synth,
                                            fract32 sustain);
void Aleph_MonoSynth_set_filter_env_release(Aleph_MonoSynth *const synth,
                                            fract32 release);
void Aleph_MonoSynth_set_filter_env_depth(Aleph_MonoSynth *const synth,
                                          fract32 depth);

void Aleph_MonoSynth_set_pitch_env_attack(Aleph_MonoSynth *const synth,
                                          fract32 attack);
void Aleph_MonoSynth_set_pitch_env_decay(Aleph_MonoSynth *const synth,
                                         fract32 decay);
void Aleph_MonoSynth_set_pitch_env_sustain(Aleph_MonoSynth *const synth,
                                           fract32 sustain);
void Aleph_MonoSynth_set_pitch_env_release(Aleph_MonoSynth *const synth,
                                           fract32 release);
void Aleph_MonoSynth_set_pitch_env_depth(Aleph_MonoSynth *const synth,
                                         fract32 depth);

void Aleph_MonoSynth_set_amp_lfo_freq(Aleph_MonoSynth *const synth,
                                      fract32 freq);
void Aleph_MonoSynth_set_amp_lfo_depth(Aleph_MonoSynth *const synth,
                                       fract32 depth);

void Aleph_MonoSynth_set_filter_lfo_freq(Aleph_MonoSynth *const synth,
                                         fract32 freq);
void Aleph_MonoSynth_set_filter_lfo_depth(Aleph_MonoSynth *const synth,
                                          fract32 depth);

void Aleph_MonoSynth_set_pitch_lfo_freq(Aleph_MonoSynth *const synth,
                                        fract32 freq);
void Aleph_MonoSynth_set_pitch_lfo_depth(Aleph_MonoSynth *const synth,
                                         fract32 depth);

void Aleph_MonoSynth_set_gate(Aleph_MonoSynth *const synth, bool gate);
void Aleph_MonoSynth_set_phase_reset(Aleph_MonoSynth *const synth, bool reset);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
