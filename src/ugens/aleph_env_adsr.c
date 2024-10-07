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
 * @file    aleph_env_adsr.c
 *
 * @brief   ADSR Envelope.
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_env_adsr.h"
#include "aleph_interpolate.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_EnvADSR_init(Aleph_EnvADSR *const envelope, t_Aleph *const aleph) {

    Aleph_EnvADSR_init_to_pool(envelope, &aleph->mempool);
}

void Aleph_EnvADSR_init_to_pool(Aleph_EnvADSR *const envelope,
                                Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_EnvADSR *env = *envelope =
        (t_Aleph_EnvADSR *)mpool_alloc(sizeof(t_Aleph_EnvADSR), mp);

    env->mempool = mp;

    env->env_state = ADSR_RELEASE;
    env->env_out = 0;
    env->overshoot = FR32_MAX / 10;
    env->attack_time = SLEW_10MS;
    env->decay_time = SLEW_100MS;
    env->sustain_level = FR32_MAX >> 2;
    env->release_time = SLEW_1S;
}

void Aleph_EnvADSR_free(Aleph_EnvADSR *const envelope) {

    t_Aleph_EnvADSR *env = *envelope;

    mpool_free((char *)env, env->mempool);
}

void Aleph_EnvADSR_set_gate(Aleph_EnvADSR *const envelope, bool gate) {

    t_Aleph_EnvADSR *env = *envelope;

    if (gate) {
        env->env_state = ADSR_ATTACK;

    } else {
        env->env_state = ADSR_RELEASE;
    }
}

fract32 Aleph_EnvADSR_next(Aleph_EnvADSR *envelope) {

    t_Aleph_EnvADSR *env = *envelope;

    switch (env->env_state) {

    case ADSR_ATTACK:
        normalised_log_slew(&(env->env_out), FR32_MAX, env->attack_time);
        if (env->env_out > FR32_MAX - env->overshoot) {
            env->env_state = ADSR_DECAY;
        }
        break;

    case ADSR_DECAY:
        normalised_log_slew(&(env->env_out), env->sustain_level,
                            env->decay_time);
        break;

    case ADSR_RELEASE:
        normalised_log_slew(&(env->env_out), 0, env->release_time);
        break;
    }

    return env->env_out;
}

void Aleph_EnvADSR_set_attack(Aleph_EnvADSR *const envelope, fract32 attack) {

    t_Aleph_EnvADSR *env = *envelope;

    env->attack_time = attack;
}

void Aleph_EnvADSR_set_decay(Aleph_EnvADSR *const envelope, fract32 decay) {

    t_Aleph_EnvADSR *env = *envelope;

    env->decay_time = decay;
}

void Aleph_EnvADSR_set_sustain(Aleph_EnvADSR *const envelope, fract32 sustain) {

    t_Aleph_EnvADSR *env = *envelope;

    env->sustain_level = sustain;
}

void Aleph_EnvADSR_set_release(Aleph_EnvADSR *const envelope, fract32 release) {

    t_Aleph_EnvADSR *env = *envelope;

    env->release_time = release;
}

void Aleph_EnvADSR_16_init(Aleph_EnvADSR_16 *const envelope,
                           t_Aleph *const aleph) {

    Aleph_EnvADSR_16_init_to_pool(envelope, &aleph->mempool);
}

void Aleph_EnvADSR_16_init_to_pool(Aleph_EnvADSR_16 *const envelope,
                                   Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_Aleph_EnvADSR_16 *env = *envelope =
        (t_Aleph_EnvADSR_16 *)mpool_alloc(sizeof(t_Aleph_EnvADSR_16), mp);

    env->mempool = mp;

    env->env_state = ADSR_RELEASE;
    env->env_out = 0;
    env->overshoot = FR16_MAX / 10;
    env->attack_time = SLEW_10MS_16;
    env->decay_time = SLEW_100MS_16;
    env->sustain_level = FR16_MAX >> 2;
    env->release_time = SLEW_1S_16;
}

void Aleph_EnvADSR_16_set_gate(Aleph_EnvADSR_16 *const envelope, bool gate) {

    t_Aleph_EnvADSR_16 *env = *envelope;

    if (gate) {
        env->env_state = ADSR_ATTACK;

    } else {
        env->env_state = ADSR_RELEASE;
    }
}

fract16 Aleph_EnvADSR_16_next(Aleph_EnvADSR_16 *envelope) {

    t_Aleph_EnvADSR_16 *env = *envelope;

    switch (env->env_state) {

    case ADSR_ATTACK:
        normalised_log_slew_16(&(env->env_out), FR16_MAX, env->attack_time);
        if (env->env_out > FR16_MAX - env->overshoot) {
            env->env_state = ADSR_DECAY;
        }
        break;

    case ADSR_DECAY:
        normalised_log_slew_16(&(env->env_out), env->sustain_level,
                               env->decay_time);
        break;

    case ADSR_RELEASE:
        normalised_log_slew_16(&(env->env_out), 0, env->release_time);
        break;
    }

    return env->env_out;
}

void Aleph_EnvADSR_16_set_attack(Aleph_EnvADSR_16 *const envelope,
                                 fract16 attack) {

    t_Aleph_EnvADSR_16 *env = *envelope;

    env->attack_time = attack;
}

void Aleph_EnvADSR_16_set_decay(Aleph_EnvADSR_16 *const envelope,
                                fract16 decay) {

    t_Aleph_EnvADSR_16 *env = *envelope;

    env->decay_time = decay;
}

void Aleph_EnvADSR_16_set_sustain(Aleph_EnvADSR_16 *const envelope,
                                  fract16 sustain) {

    t_Aleph_EnvADSR_16 *env = *envelope;

    env->sustain_level = sustain;
}

void Aleph_EnvADSR_16_set_release(Aleph_EnvADSR_16 *const envelope,
                                  fract16 release) {

    t_Aleph_EnvADSR_16 *env = *envelope;

    env->release_time = release;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
