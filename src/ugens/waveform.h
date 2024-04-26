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
 * @file    waveform.h
 *
 * @brief   Public ASPI for anti-aliased basic waveform oscillators.
 *
 */

#ifndef ALEPH_WAVEFORM_H
#define ALEPH_WAVEFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros and Definitions ---------------------------------------*/

typedef struct {
    uint8_t shape;
    fract32 freq;
    fract32 phase;
} t_Waveform;

/*----- Extern variable declarations ---------------------------------*/

void Waveform_init(t_Waveform *wave, t_Aleph *aleph);

/*----- Extern function prototypes -----------------------------------*/

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
