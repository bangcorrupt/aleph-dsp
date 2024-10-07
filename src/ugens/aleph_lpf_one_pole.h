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
 * @file    aleph_lpf_one_pole.h
 *
 * @brief   Publice API for one pole low pass filter.
 */

#ifndef ALEPH_LPF_ONE_POLE_H
#define ALEPH_LPF_ONE_POLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

#include "aleph_interpolate.h"

/*----- Macros -------------------------------------------------------*/

#define ALEPH_FILTER_ONE_POLE_DEFAULT_COEFF (SLEW_1MS)
#define ALEPH_FILTER_ONE_POLE_DEFAULT_TARGET (0)
#define ALEPH_FILTER_ONE_POLE_DEFAULT_OUTPUT (0)

/*----- Typedefs -----------------------------------------------------*/

typedef struct {
    Mempool mempool;
    fract32 coeff;  // integration coefficient
    fract32 target; // target value
    fract32 output; // filtered value
} t_Aleph_LPFOnePole;

typedef t_Aleph_LPFOnePole *Aleph_LPFOnePole;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_LPFOnePole_init(Aleph_LPFOnePole *const lpf, t_Aleph *const aleph);

void Aleph_LPFOnePole_init_to_pool(Aleph_LPFOnePole *const lpf,
                                   Mempool *const mempool);

void Aleph_LPFOnePole_free(Aleph_LPFOnePole *const lpf);

fract32 Aleph_LPFOnePole_next(Aleph_LPFOnePole *const lpf);
fract32 Aleph_LPFOnePole_norm_next(Aleph_LPFOnePole *const lpf);
void Aleph_LPFOnePole_set_target(Aleph_LPFOnePole *const lpf, fract32 target);
void Aleph_LPFOnePole_set_output(Aleph_LPFOnePole *const lpf, fract32 output);
void Aleph_LPFOnePole_set_coeff(Aleph_LPFOnePole *const lpf, fract32 coeff);
bool Aleph_LPFOnePole_sync(Aleph_LPFOnePole *const lpf);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
