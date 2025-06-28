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
 * @file    aleph_fm_voice.h
 *
 * @brief   Public API for FM voice.
 */

#ifndef ALEPH_FM_VOICE_H
#define ALEPH_FM_VOICE_H

#include "aleph.h"
#include "aleph_env_adsr.h"
#include "aleph_phasor.h"
#include "fract_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

/*----- Macros -------------------------------------------------------*/

#define ALEPH_FM_OPS_MAX 8
#define ALEPH_FM_MOD_POINTS_MAX 8

#define ALEPH_FM_DEFAULT_NUM_OPS 4
#define ALEPH_FM_DEFAULT_NUM_MOD_POINTS 2

#define ALEPH_FM_OVERSAMPLE_BITS 2
#define ALEPH_FM_OVERSAMPLE (1 << ALEPH_FM_OVERSAMPLE_BITS)

/* #define fm_smooth ((fract16) (fr16_max * ((4.0 * pi * fm_oversample) / (1.0
 * + 4.0 * pi * fm_oversample)))) */

#define ALEPH_FM_SMOOTH ((fract16)(FR16_MAX * 0.7))

/*----- Typedefs -----------------------------------------------------*/

typedef struct {

    Mempool mempool;

    uint8_t num_ops;
    uint8_t num_mod_points;

    fix16 note_freq;
    fix16 note_tune;
    fract32 base_freq;
    fract32 portamento;

    Aleph_Phasor op_osc[ALEPH_FM_OPS_MAX];
    Aleph_EnvADSR op_env[ALEPH_FM_OPS_MAX];

    fract32 op_tune[ALEPH_FM_OPS_MAX];
    uint8_t op_mod1_source[ALEPH_FM_OPS_MAX];
    fract16 op_mod1_gain[ALEPH_FM_OPS_MAX];
    uint8_t op_mod2_source[ALEPH_FM_OPS_MAX];
    fract16 op_mod2_gain[ALEPH_FM_OPS_MAX];

    fract16 op_mod_last[ALEPH_FM_OPS_MAX];
    fract32 op_freqs[ALEPH_FM_OPS_MAX];
    fract32 op_slew[ALEPH_FM_OPS_MAX];
    uint8_t band_limit[ALEPH_FM_OPS_MAX];
    uint8_t freq_saturate[ALEPH_FM_OPS_MAX];

    fract16 op_outputs_internal[ALEPH_FM_OPS_MAX];
    fract16 op_outputs[ALEPH_FM_OPS_MAX];

    fract32 op_mod_points_external[ALEPH_FM_MOD_POINTS_MAX];
    fract32 op_mod_points_last[ALEPH_FM_MOD_POINTS_MAX];

} t_Aleph_FMVoice;

typedef t_Aleph_FMVoice *Aleph_FMVoice;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_FMVoice_init(Aleph_FMVoice *const fm_voice, t_Aleph *const aleph);

void Aleph_FMVoice_init_to_pool(Aleph_FMVoice *const fm_voice,
                                Mempool *const mempool);

void Aleph_FMVoice_next(Aleph_FMVoice *const fm_voice);

void Aleph_FMVoice_set_gate(Aleph_FMVoice *const fm_voice, bool gate);

void Aleph_FMVoice_set_note_freq(Aleph_FMVoice *const fm_voice, fix16 freq);
void Aleph_FMVoice_set_note_tune(Aleph_FMVoice *const fm_voice, fix16 tune);

void Aleph_FMVoice_set_op_tune(Aleph_FMVoice *const fm_voice, uint8_t op_index,
                               fract32 tune);

void Aleph_FMVoice_set_op_mod1_gain(Aleph_FMVoice *const fm_voice,
                                    uint8_t op_index, fract16 gain);

void Aleph_FMVoice_set_op_mod2_gain(Aleph_FMVoice *const fm_voice,
                                    uint8_t op_index, fract16 gain);

void Aleph_FMVoice_set_op_attack(Aleph_FMVoice *const fm_voice,
                                 uint8_t op_index, fract32 attack);

void Aleph_FMVoice_set_op_decay(Aleph_FMVoice *const fm_voice, uint8_t op_index,
                                fract32 decay);

void Aleph_FMVoice_set_op_sustain(Aleph_FMVoice *const fm_voice,
                                  uint8_t op_index, fract32 sustain);

void Aleph_FMVoice_set_op_release(Aleph_FMVoice *const fm_voice,
                                  uint8_t op_index, fract32 release);

void Aleph_FMVoice_set_portamento(Aleph_FMVoice *const fm_voice,
                                  uint8_t op_index, fract32 portamento);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
