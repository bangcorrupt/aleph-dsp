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
 * @file    aleph_monovoice.c
 *
 * @brief   Monophonic synth voice module.
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_env_adsr.h"
#include "aleph_filter.h"
#include "aleph_filter_svf.h"
#include "aleph_lpf_one_pole.h"
#include "aleph_oscillator.h"
#include "aleph_waveform.h"

#include "aleph_monovoice.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_MonoVoice_init(Aleph_MonoVoice *const synth, t_Aleph *const aleph) {

    Aleph_MonoVoice_init_to_pool(synth, &aleph->mempool);
}

void Aleph_MonoVoice_init_to_pool(Aleph_MonoVoice *const synth,
                                  Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_MonoVoice *syn = *synth =
        (t_Aleph_MonoVoice *)mpool_alloc(sizeof(t_Aleph_MonoVoice), mp);

    syn->mempool = mp;

    syn->freq_offset = ALEPH_MONOVOICE_DEFAULT_FREQ_OFFSET;
    syn->filter_type = ALEPH_MONOVOICE_DEFAULT_FILTER_TYPE;

    syn->phase_reset = ALEPH_MONOVOICE_DEFAULT_PHASE_RESET;

    Aleph_WaveformDual_init_to_pool(&syn->waveform, mempool);

    Aleph_FilterSVF_init_to_pool(&syn->filter, mempool);

    Aleph_HPF_init_to_pool(&syn->dc_block, mempool);

    Aleph_LPFOnePole_init_to_pool(&syn->freq_slew, mempool);
    Aleph_LPFOnePole_set_output(&syn->freq_slew, ALEPH_MONOVOICE_DEFAULT_FREQ);

    Aleph_LPFOnePole_init_to_pool(&syn->cutoff_slew, mempool);
    Aleph_LPFOnePole_set_output(&syn->cutoff_slew,
                                ALEPH_MONOVOICE_DEFAULT_CUTOFF);

    Aleph_LPFOnePole_init_to_pool(&syn->amp_slew, mempool);
    Aleph_LPFOnePole_set_output(&syn->amp_slew, ALEPH_MONOVOICE_DEFAULT_AMP);
}

void Aleph_MonoVoice_free(Aleph_MonoVoice *const synth) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_WaveformDual_free(&syn->waveform);

    Aleph_FilterSVF_free(&syn->filter);

    Aleph_HPF_free(&syn->dc_block);

    Aleph_LPFOnePole_free(&syn->freq_slew);
    Aleph_LPFOnePole_free(&syn->cutoff_slew);

    mpool_free((char *)syn, syn->mempool);
}

fract32 Aleph_MonoVoice_next(Aleph_MonoVoice *const synth) {

    t_Aleph_MonoVoice *syn = *synth;

    fract32 output;

    fract32 amp;
    fract32 freq;
    fract32 cutoff;

    // Get slewed frequency.
    freq = Aleph_LPFOnePole_next(&syn->freq_slew);

    /// TODO: Set oscillator type (Dual, Unison, etc...).

    // Set oscillator frequency.
    Aleph_WaveformDual_set_freq_a(&syn->waveform, freq);
    Aleph_WaveformDual_set_freq_b(&syn->waveform,
                                  fix16_mul_fract(freq, syn->freq_offset));

    // Generate waveforms.
    output = Aleph_WaveformDual_next(&syn->waveform);

    // Shift right to prevent clipping.
    output = shr_fr1x32(output, 1);

    // Get slewed amplitude.
    amp = Aleph_LPFOnePole_next(&syn->amp_slew);

    // Apply amp modulation.
    output = mult_fr1x32x32(output, amp);

    // Get slewed cutoff.
    cutoff = Aleph_LPFOnePole_next(&syn->cutoff_slew);

    // Set filter cutoff.
    Aleph_FilterSVF_set_coeff(&syn->filter, cutoff);

    // Apply filter.
    switch (syn->filter_type) {

    case ALEPH_FILTERSVF_TYPE_LPF:
        output = Aleph_FilterSVF_sc_os_lpf_next(&syn->filter, output);
        break;

    case ALEPH_FILTERSVF_TYPE_BPF:
        output = Aleph_FilterSVF_sc_os_bpf_next(&syn->filter, output);
        break;

    case ALEPH_FILTERSVF_TYPE_HPF:
        output = Aleph_FilterSVF_sc_os_hpf_next(&syn->filter, output);
        break;

    default:
        // Default to LPF.
        output = Aleph_FilterSVF_sc_os_lpf_next(&syn->filter, output);
        break;
    }

    // Block DC.
    output = Aleph_HPF_dc_block(&syn->dc_block, output);

    return output;
}

void Aleph_MonoVoice_set_shape(Aleph_MonoVoice *const synth,
                               e_Aleph_Waveform_shape shape) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_WaveformDual_set_shape(&syn->waveform, shape);
}

void Aleph_MonoVoice_set_amp(Aleph_MonoVoice *const synth, fract32 amp) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_LPFOnePole_set_target(&syn->amp_slew, amp);
}

void Aleph_MonoVoice_set_phase(Aleph_MonoVoice *const synth, fract32 phase) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_WaveformDual_set_phase(&syn->waveform, phase);
}

void Aleph_MonoVoice_set_freq(Aleph_MonoVoice *const synth, fract32 freq) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_LPFOnePole_set_target(&syn->freq_slew, freq);
}

void Aleph_MonoVoice_set_freq_offset(Aleph_MonoVoice *const synth,
                                     fract32 freq_offset) {

    t_Aleph_MonoVoice *syn = *synth;

    syn->freq_offset = freq_offset;
}

void Aleph_MonoVoice_set_filter_type(Aleph_MonoVoice *const synth,
                                     e_Aleph_FilterSVF_type type) {

    t_Aleph_MonoVoice *syn = *synth;

    syn->filter_type = type;
}

void Aleph_MonoVoice_set_cutoff(Aleph_MonoVoice *const synth, fract32 cutoff) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_LPFOnePole_set_target(&syn->cutoff_slew, cutoff);
}

void Aleph_MonoVoice_set_res(Aleph_MonoVoice *const synth, fract32 res) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_FilterSVF_set_rq(&syn->filter, res);
}

void Aleph_MonoVoice_set_phase_reset(Aleph_MonoVoice *const synth, bool reset) {

    t_Aleph_MonoVoice *syn = *synth;

    syn->phase_reset = reset;
}

void Aleph_MonoVoice_set_gate(Aleph_MonoVoice *const synth, bool gate) {

    t_Aleph_MonoVoice *syn = *synth;

    if (syn->phase_reset && gate) {
        Aleph_WaveformDual_set_phase(&syn->waveform, 0);
    }
}

void Aleph_MonoVoice_set_amp_slew(Aleph_MonoVoice *const synth,
                                  fract32 amp_slew) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_LPFOnePole_set_coeff(&syn->amp_slew, amp_slew);
}

void Aleph_MonoVoice_set_freq_slew(Aleph_MonoVoice *const synth,
                                   fract32 freq_slew) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_LPFOnePole_set_coeff(&syn->freq_slew, freq_slew);
}

void Aleph_MonoVoice_set_cutoff_slew(Aleph_MonoVoice *const synth,
                                     fract32 cutoff_slew) {

    t_Aleph_MonoVoice *syn = *synth;

    Aleph_LPFOnePole_set_coeff(&syn->cutoff_slew, cutoff_slew);
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
