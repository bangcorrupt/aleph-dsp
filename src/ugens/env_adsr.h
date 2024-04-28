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
 * @file    template.h
 *
 * @brief   Template for header files.
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
} e_EnvADSR_state;

typedef struct {
    Mempool mempool;
    e_EnvADSR_state env_state;
    fract32 env_out;
    fract32 overshoot;
    fract32 attack_time;
    fract32 decay_time;
    fract32 sustain_level;
    fract32 release_time;
} t_EnvADSR;

typedef t_EnvADSR *EnvADSR;

typedef struct {
    Mempool mempool;
    e_EnvADSR_state env_state;
    fract16 env_out;
    fract16 overshoot;
    fract16 attack_time;
    fract16 decay_time;
    fract16 sustain_level;
    fract16 release_time;
} t_EnvADSR_16;

typedef t_EnvADSR_16 *EnvADSR_16;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void EnvADSR_init(EnvADSR *const envelope, t_Aleph *const aleph);
void EnvADSR_init_to_pool(EnvADSR *const envelope, Mempool *const mempool);
void EnvADSR_free(EnvADSR *const envelope);

void EnvADSR_press(EnvADSR *const envelope);
void EnvADSR_release(EnvADSR *const envelope);
fract32 EnvADSR_next(EnvADSR *const envelope);

void EnvADSR_set_attack(EnvADSR *const envelope, fract32 attack);
void EnvADSR_set_decay(EnvADSR *const envelope, fract32 decay);
void EnvADSR_set_sustain(EnvADSR *const envelope, fract32 sustain);
void EnvADSR_set_release(EnvADSR *const envelope, fract32 release);

void EnvADSR_16_init(EnvADSR_16 *const envelope, t_Aleph *const aleph);
void EnvADSR_16_init_to_pool(EnvADSR_16 *const envelope,
                             Mempool *const mempool);

void EnvADSR_16_free(EnvADSR *const envelope);

void EnvADSR_16_press(EnvADSR_16 *const envelope);
void EnvADSR_16_release(EnvADSR_16 *const envelope);
fract16 EnvADSR_16_next(EnvADSR_16 *const envelope);

void EnvADSR_16_set_attack(EnvADSR_16 *const envelope, fract16 attack);
void EnvADSR_16_set_decay(EnvADSR_16 *const envelope, fract16 decay);
void EnvADSR_16_set_sustain(EnvADSR_16 *const envelope, fract16 sustain);
void EnvADSR_16_set_release(EnvADSR_16 *const envelope, fract16 release);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
