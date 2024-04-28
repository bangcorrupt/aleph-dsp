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
 * @file    lpf_one_pole.h
 *
 * @brief   Publice API for one pole low pass filter.
 *
 */

#ifndef ALEPH_LPF_ONE_POLE_H
#define ALEPH_LPF_ONE_POLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "interpolate.h"

/*----- Macros and Definitions ---------------------------------------*/

#define FILTER_ONE_POLE_DEFAULT_COEFF (SLEW_1MS)

typedef struct {
    Mempool mempool;
    fract32 coeff;  // integration coefficient
    fract32 target; // target value
    fract32 output; // filtered value
} t_LPFOnePole;

typedef t_LPFOnePole *LPFOnePole;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void LPFOnePole_init(LPFOnePole *const lpf, fract32 input,
                     t_Aleph *const aleph);

void LPFOnePole_init_to_pool(LPFOnePole *const lpf, fract32 input,
                             Mempool *const mempool);

fract32 LPFOnePole_free(LPFOnePole *const lpf);

fract32 LPFOnePole_next(LPFOnePole *const lpf);
fract32 LPFOnePole_norm_next(LPFOnePole *const lpf);
void LPFOnePole_set_input(LPFOnePole *const lpf, fract32 input);
void LPFOnePole_set_coeff(LPFOnePole *const lpf, fract32 coeff);
bool LPFOnePole_sync(LPFOnePole *const lpf);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
