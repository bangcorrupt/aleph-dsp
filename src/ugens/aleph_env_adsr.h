/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/*
 * @file    aleph_env_adsr.h
 *
 * @brief   Public API for ADSR envelope.
 *
 */

#ifndef ALEPH_ENV_ADSR_H
#define ALEPH_ENV_ADSR_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef enum {
    ADSR_ATTACK,
    ADSR_DECAY,
    ADSR_RELEASE,
} e_Aleph_EnvADSR_state;

typedef struct {
    Mempool mempool;
    e_Aleph_EnvADSR_state env_state;
    fract32 env_out;
    fract32 overshoot;
    fract32 attack_time;
    fract32 decay_time;
    fract32 sustain_level;
    fract32 release_time;
} t_Aleph_EnvADSR;

typedef t_Aleph_EnvADSR *Aleph_EnvADSR;

typedef struct {
    Mempool mempool;
    e_Aleph_EnvADSR_state env_state;
    fract16 env_out;
    fract16 overshoot;
    fract16 attack_time;
    fract16 decay_time;
    fract16 sustain_level;
    fract16 release_time;
} t_Aleph_EnvADSR_16;

typedef t_Aleph_EnvADSR_16 *Aleph_EnvADSR_16;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_EnvADSR_init(Aleph_EnvADSR *const envelope, t_Aleph *const aleph);
void Aleph_EnvADSR_init_to_pool(Aleph_EnvADSR *const envelope,
                                Mempool *const mempool);
void Aleph_EnvADSR_free(Aleph_EnvADSR *const envelope);

fract32 Aleph_EnvADSR_next(Aleph_EnvADSR *const envelope);

void Aleph_EnvADSR_set_gate(Aleph_EnvADSR *const envelope, bool gate);
void Aleph_EnvADSR_set_attack(Aleph_EnvADSR *const envelope, fract32 attack);
void Aleph_EnvADSR_set_decay(Aleph_EnvADSR *const envelope, fract32 decay);
void Aleph_EnvADSR_set_sustain(Aleph_EnvADSR *const envelope, fract32 sustain);
void Aleph_EnvADSR_set_release(Aleph_EnvADSR *const envelope, fract32 release);

void Aleph_EnvADSR_16_init(Aleph_EnvADSR_16 *const envelope,
                           t_Aleph *const aleph);
void Aleph_EnvADSR_16_init_to_pool(Aleph_EnvADSR_16 *const envelope,
                                   Mempool *const mempool);

void Aleph_EnvADSR_16_free(Aleph_EnvADSR *const envelope);

fract16 Aleph_EnvADSR_16_next(Aleph_EnvADSR_16 *const envelope);

void Aleph_EnvADSR_16_set_gate(Aleph_EnvADSR_16 *const envelope, bool gate);
void Aleph_EnvADSR_16_set_attack(Aleph_EnvADSR_16 *const envelope,
                                 fract16 attack);
void Aleph_EnvADSR_16_set_decay(Aleph_EnvADSR_16 *const envelope,
                                fract16 decay);
void Aleph_EnvADSR_16_set_sustain(Aleph_EnvADSR_16 *const envelope,
                                  fract16 sustain);
void Aleph_EnvADSR_16_set_release(Aleph_EnvADSR_16 *const envelope,
                                  fract16 release);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
