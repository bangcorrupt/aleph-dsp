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
 * @file    aleph_fm_voice.c
 *
 * @brief   FM voice.
 */

/*----- Includes -----------------------------------------------------*/

#include "cdefBF52x_base.h"
#include "fix.h"
#include "fix16_fract.h"
#include "fract_math.h"

#include "aleph_env_adsr.h"
#include "aleph_interpolate.h"
#include "aleph_lpf_one_pole.h"
#include "aleph_osc_polyblep.h"

#include "aleph_fm_voice.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_FMVoice_init(Aleph_FMVoice *const fm_voice, t_Aleph *const aleph) {

    Aleph_FMVoice_init_to_pool(fm_voice, &aleph->mempool);
}

void Aleph_FMVoice_init_to_pool(Aleph_FMVoice *const fm_voice,
                                Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_FMVoice *fmv = *fm_voice =
        (t_Aleph_FMVoice *)mpool_alloc(sizeof(t_Aleph_FMVoice), mp);

    fmv->num_ops = ALEPH_FM_DEFAULT_NUM_OPS;
    fmv->num_mod_points = ALEPH_FM_DEFAULT_NUM_MOD_POINTS;
    fmv->note_freq = 110 << 16;
    fmv->note_tune = FIX16_ONE;
    fmv->portamento = SLEW_1MS;
    fmv->base_freq = 0;

    int i;
    for (i = 0; i < fmv->num_ops; i++) {
        fmv->op_tune[i] = FIX16_ONE;
        fmv->op_mod1_source[i] = i;
        fmv->op_mod1_gain[i] = 0;

        /// TODO: Investigate with a debugger.
        ///         The original modulo operation was causing a crash,
        ///         probably due to a negative array index.
        ///         I think the conditional below is the intended behaviour.
        //
        // fmv->opMod2Source[i] = (i - 1) % fmv->nOps;
        fmv->op_mod2_source[i] = i == 0 ? fmv->num_ops - 1 : i - 1;

        fmv->op_mod2_gain[i] = 0;
        fmv->op_outputs[i] = 0;
        fmv->op_outputs_internal[i] = 0;

        Aleph_Phasor_init_to_pool(&(fmv->op_osc[i]), mempool);

        fmv->op_freqs[i] = 0;

        /// TODO: Either use 16 bit envelope,
        ///         or have 32 bit outputs.
        ///         Currently truncating output of 32 bit envelope.
        //
        Aleph_EnvADSR_init_to_pool(&(fmv->op_env[i]), mempool);

        fmv->op_mod_last[i] = 0;
        fmv->band_limit[i] = 1;
        fmv->freq_saturate[i] = 1;
        fmv->op_slew[i] = SLEW_1MS;
    }
    for (i = 0; i < ALEPH_FM_MOD_POINTS_MAX; i++) {
        fmv->op_mod_points_external[i] = 0;
        fmv->op_mod_points_last[i] = 0;
    }
}

void Aleph_FMVoice_next(Aleph_FMVoice *const fm_voice) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    int i, j;

    fract16 oversample_outs[ALEPH_FM_OPS_MAX][ALEPH_FM_OVERSAMPLE];

    fract16 mod_points[ALEPH_FM_MOD_POINTS_MAX];

    fract32 op_freq_target;
    fract16 env_next[ALEPH_FM_OPS_MAX];

    fract16 op_mod;
    fract32 op_phase;
    fract16 osc_signal;
    fract16 next_op_outputs[ALEPH_FM_OPS_MAX];

    normalised_log_slew(&(fmv->base_freq),
                        fix16_mul_fract(fmv->note_freq, fmv->note_tune),
                        fmv->portamento);

    for (i = 0; i < fmv->num_ops; i++) {

        env_next[i] = trunc_fr1x32(Aleph_EnvADSR_next(&(fmv->op_env[i])));

        op_freq_target =
            shr_fr1x32(fix16_mul_fract(fmv->base_freq, fmv->op_tune[i]),
                       ALEPH_FM_OVERSAMPLE_BITS);

        normalised_log_slew(&(fmv->op_freqs[i]), op_freq_target,
                            fmv->op_slew[i]);
    }

    for (i = 0; i < fmv->num_mod_points; i++) {

        mod_points[i] = trunc_fr1x32(fmv->op_mod_points_external[i]);
    }

    for (j = 0; j < ALEPH_FM_OVERSAMPLE; j++) {

        // Calculate modulation point & bandLimit to 20khz or so...
        for (i = 0; i < fmv->num_ops; i++) {

            // Add first modulation source.
            if (fmv->op_mod1_source[i] < fmv->num_ops) {

                op_mod = multr_fr1x16(
                    fmv->op_outputs_internal[fmv->op_mod1_source[i]],
                    fmv->op_mod1_gain[i]);

            } else {
                op_mod = multr_fr1x16(
                    mod_points[fmv->op_mod1_source[i] - fmv->num_ops],
                    fmv->op_mod1_gain[i]);
            }

            // Add second modulation source.
            if (fmv->op_mod2_source[i] < fmv->num_ops) {

                op_mod = add_fr1x16(
                    op_mod,
                    multr_fr1x16(
                        fmv->op_outputs_internal[fmv->op_mod2_source[i]],
                        fmv->op_mod2_gain[i]));

            } else {
                op_mod = add_fr1x16(
                    op_mod,
                    multr_fr1x16(
                        mod_points[fmv->op_mod2_source[i] - fmv->num_ops],
                        fmv->op_mod2_gain[i]));
            }

            op_mod = shr_fr1x32(op_mod, 2);

            if (fmv->band_limit[i]) {

                // BandLimit modulation signal with 20khz iir.
                op_mod = mult_fr1x16(op_mod, FR16_MAX - ALEPH_FM_SMOOTH);

                op_mod = add_fr1x16(
                    op_mod, multr_fr1x16(fmv->op_mod_last[i], ALEPH_FM_SMOOTH));

                fmv->op_mod_last[i] = op_mod;
            }

            // Phase increment each op with the oversample-compensated
            // frequency, calculate the op output for next oversampled frame.
            op_phase =
                Aleph_Phasor_next_dynamic(&(fmv->op_osc[i]), fmv->op_freqs[i]);

            if (fmv->freq_saturate[i]) {

                op_phase += shl_fr1x32(op_mod, 20);

            } else {
                op_phase += (op_mod << 20);
            }

            osc_signal = sine_polyblep(op_phase);
            next_op_outputs[i] = multr_fr1x16(env_next[i], osc_signal);
        }

        for (i = 0; i < fmv->num_ops; i++) {

            // shuffle the op outputs into the buffer for next oversampled frame
            fmv->op_outputs_internal[i] = next_op_outputs[i];
            // set up the oversampled output to sum & average
            oversample_outs[i][j] =
                shr_fr1x16(next_op_outputs[i], ALEPH_FM_OVERSAMPLE_BITS);
        }
    }

    // convert the oversampled output back to original sample rate
    for (i = 0; i < fmv->num_ops; i++) {

        fmv->op_outputs[i] = 0;

        for (j = 0; j < ALEPH_FM_OVERSAMPLE; j++) {

            fmv->op_outputs[i] =
                add_fr1x16(oversample_outs[i][j], fmv->op_outputs[i]);
        }
    }
}

void Aleph_FMVoice_set_note_freq(Aleph_FMVoice *const fm_voice, fix16 freq) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    fmv->note_freq = freq;
}

void Aleph_FMVoice_set_note_tune(Aleph_FMVoice *const fm_voice, fix16 tune) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    fmv->note_tune = tune;
}

void Aleph_FMVoice_set_gate(Aleph_FMVoice *const fm_voice, bool gate) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    int i;
    for (i = 0; i < fmv->num_ops; i++) {

        Aleph_EnvADSR_set_gate(&(fmv->op_env[i]), gate);
    }
}

void Aleph_FMVoice_set_op_tune(Aleph_FMVoice *const fm_voice, uint8_t op_index,
                               fract32 tune) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    fmv->op_tune[op_index] = tune;
}

void Aleph_FMVoice_set_op_mod1_gain(Aleph_FMVoice *const fm_voice,
                                    uint8_t op_index, fract16 gain) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    fmv->op_mod1_gain[op_index] = gain;
}

void Aleph_FMVoice_set_op_mod2_gain(Aleph_FMVoice *const fm_voice,
                                    uint8_t op_index, fract16 gain) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    fmv->op_mod2_gain[op_index] = gain;
}

void Aleph_FMVoice_set_op_attack(Aleph_FMVoice *const fm_voice,
                                 uint8_t op_index, fract32 attack) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    Aleph_EnvADSR_set_attack(&(fmv->op_env[op_index]), attack);
}

void Aleph_FMVoice_set_op_decay(Aleph_FMVoice *const fm_voice, uint8_t op_index,
                                fract32 decay) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    Aleph_EnvADSR_set_decay(&(fmv->op_env[op_index]), decay);
}

void Aleph_FMVoice_set_op_sustain(Aleph_FMVoice *const fm_voice,
                                  uint8_t op_index, fract32 sustain) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    Aleph_EnvADSR_set_sustain(&(fmv->op_env[op_index]), sustain);
}

void Aleph_FMVoice_set_op_release(Aleph_FMVoice *const fm_voice,
                                  uint8_t op_index, fract32 release) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    Aleph_EnvADSR_set_release(&(fmv->op_env[op_index]), release);
}

void Aleph_FMVoice_set_portamento(Aleph_FMVoice *const fm_voice,
                                  uint8_t op_index, fract32 portamento) {

    t_Aleph_FMVoice *fmv = *fm_voice;

    fmv->portamento = portamento;
}

/*----- Static function implementations ------------------------------*/

/*----- END OF FILE --------------------------------------------------*/
