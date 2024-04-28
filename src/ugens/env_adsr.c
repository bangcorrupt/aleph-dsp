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
 * @file    EnvADSR.c
 *
 * @brief   ADSR Envelope.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph-mempool.h"
#include "aleph.h"

#include "ugens/env_adsr.h"
#include "ugens/interpolate.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void EnvADSR_init(EnvADSR *const envelope, t_Aleph *const aleph) {

    EnvADSR_init_to_pool(envelope, &aleph->mempool);
}

void EnvADSR_init_to_pool(EnvADSR *const envelope, Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_EnvADSR *env = *envelope =
        (t_EnvADSR *)mpool_alloc(sizeof(t_EnvADSR), mp);

    env->mempool = mp;

    env->env_state = ADSR_RELEASE;
    env->env_out = 0;
    env->overshoot = FR32_MAX / 10;
    env->attack_time = SLEW_10MS;
    env->decay_time = SLEW_100MS;
    env->sustain_level = FR32_MAX >> 2;
    env->release_time = SLEW_1S;
}

void EnvADSR_free(EnvADSR *const envelope) {

    t_EnvADSR *env = *envelope;

    mpool_free((char *)env, env->mempool);
}

void EnvADSR_press(EnvADSR *const envelope) {

    t_EnvADSR *env = *envelope;

    env->env_state = ADSR_ATTACK;
}

void EnvADSR_release(EnvADSR *const envelope) {

    t_EnvADSR *env = *envelope;

    env->env_state = ADSR_RELEASE;
}

fract32 EnvADSR_next(EnvADSR *envelope) {

    t_EnvADSR *env = *envelope;

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

void EnvADSR_set_attack(EnvADSR *const envelope, fract32 attack) {

    t_EnvADSR *env = *envelope;

    env->attack_time = attack;
}

void EnvADSR_set_decay(EnvADSR *const envelope, fract32 decay) {

    t_EnvADSR *env = *envelope;

    env->decay_time = decay;
}

void EnvADSR_set_sustain(EnvADSR *const envelope, fract32 sustain) {

    t_EnvADSR *env = *envelope;

    env->sustain_level = sustain;
}

void EnvADSR_set_release(EnvADSR *const envelope, fract32 release) {

    t_EnvADSR *env = *envelope;

    env->release_time = release;
}

void EnvADSR_16_init(EnvADSR_16 *const envelope, t_Aleph *const aleph) {

    EnvADSR_16_init_to_pool(envelope, &aleph->mempool);
}

void EnvADSR_16_init_to_pool(EnvADSR_16 *const envelope,
                             Mempool *const mempool) {

    t_Mempool *mp = *mempool;

    t_EnvADSR_16 *env = *envelope =
        (t_EnvADSR_16 *)mpool_alloc(sizeof(t_EnvADSR_16), mp);

    env->mempool = mp;

    env->env_state = ADSR_RELEASE;
    env->env_out = 0;
    env->overshoot = FR16_MAX / 10;
    env->attack_time = SLEW_10MS_16;
    env->decay_time = SLEW_100MS_16;
    env->sustain_level = FR16_MAX >> 2;
    env->release_time = SLEW_1S_16;
}

void Env_ADSR_16_press(EnvADSR_16 *const envelope) {

    t_EnvADSR_16 *env = *envelope;

    env->env_state = ADSR_ATTACK;
}

void Env_ADSR_16_release(EnvADSR_16 *envelope) {

    t_EnvADSR_16 *env = *envelope;

    env->env_state = ADSR_RELEASE;
}

fract16 EnvADSR_16_next(EnvADSR_16 *envelope) {

    t_EnvADSR_16 *env = *envelope;

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

void EnvADSR_16_set_attack(EnvADSR_16 *const envelope, fract16 attack) {

    t_EnvADSR_16 *env = *envelope;

    env->attack_time = attack;
}

void EnvADSR_16_set_decay(EnvADSR_16 *const envelope, fract16 decay) {

    t_EnvADSR_16 *env = *envelope;

    env->decay_time = decay;
}

void EnvADSR_16_set_sustain(EnvADSR_16 *const envelope, fract16 sustain) {

    t_EnvADSR_16 *env = *envelope;

    env->sustain_level = sustain;
}

void EnvADSR_16_set_release(EnvADSR_16 *const envelope, fract16 release) {

    t_EnvADSR_16 *env = *envelope;

    env->release_time = release;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
