/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/**
 * @file    aleph_monovoice.h
 *
 * @brief   Public API for monovoice module.
 */

#ifndef ALEPH_MONOVOICE_H
#define ALEPH_MONOVOICE_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_filter.h"
#include "aleph_filter_svf.h"
#include "aleph_lpf_one_pole.h"
#include "aleph_waveform.h"

/*----- Macros -------------------------------------------------------*/

#define ALEPH_MONOVOICE_DEFAULT_AMP (0)
#define ALEPH_MONOVOICE_DEFAULT_FREQ (220 << 16)
#define ALEPH_MONOVOICE_DEFAULT_FREQ_OFFSET (0)

#define ALEPH_MONOVOICE_DEFAULT_CUTOFF (20000 << 16)
#define ALEPH_MONOVOICE_DEFAULT_RES (FR32_MAX)
#define ALEPH_MONOVOICE_DEFAULT_FILTER_TYPE ALEPH_FILTERSVF_TYPE_LPF

/*----- Typedefs -----------------------------------------------------*/

typedef struct {

    Mempool mempool;

    Aleph_WaveformDual waveform;
    fract32 freq_offset;

    Aleph_FilterSVF filter;
    e_Aleph_FilterSVF_type filter_type;

    Aleph_LPFOnePole amp_slew;
    Aleph_LPFOnePole freq_slew;
    Aleph_LPFOnePole cutoff_slew;

    Aleph_HPF dc_block;

} t_Aleph_MonoVoice;

typedef t_Aleph_MonoVoice *Aleph_MonoVoice;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_MonoVoice_init(Aleph_MonoVoice *const synth, t_Aleph *const aleph);
void Aleph_MonoVoice_init_to_pool(Aleph_MonoVoice *const synth,
                                  Mempool *const mempool);
void Aleph_MonoVoice_free(Aleph_MonoVoice *const synth);

fract32 Aleph_MonoVoice_next(Aleph_MonoVoice *const synth);

void Aleph_MonoVoice_set_shape(Aleph_MonoVoice *const synth,
                               e_Aleph_Waveform_shape shape);

void Aleph_MonoVoice_set_amp(Aleph_MonoVoice *const synth, fract32 amp);
void Aleph_MonoVoice_set_phase(Aleph_MonoVoice *const synth, fract32 phase);

void Aleph_MonoVoice_set_freq(Aleph_MonoVoice *const synth, fract32 freq);
void Aleph_MonoVoice_set_freq_offset(Aleph_MonoVoice *const synth,
                                     fract32 freq_offset);

void Aleph_MonoVoice_set_filter_type(Aleph_MonoVoice *const synth,
                                     e_Aleph_FilterSVF_type type);

void Aleph_MonoVoice_set_cutoff(Aleph_MonoVoice *const synth, fract32 cutoff);
void Aleph_MonoVoice_set_res(Aleph_MonoVoice *const synth, fract32 res);

void Aleph_MonoVoice_set_amp_slew(Aleph_MonoVoice *const synth,
                                  fract32 amp_slew);

void Aleph_MonoVoice_set_freq_slew(Aleph_MonoVoice *const synth,
                                   fract32 freq_slew);

void Aleph_MonoVoice_set_cutoff_slew(Aleph_MonoVoice *const synth,
                                     fract32 cutoff_slew);

void Aleph_MonoVoice_next_block(Aleph_MonoVoice *const synth, fract32 *output,
                                size_t size);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
