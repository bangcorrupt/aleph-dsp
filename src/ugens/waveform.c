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
 * @file    waveform.c
 *
 * @brief   Anti-aliased basic waveform oscillators.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph-mempool.h"
#include "aleph.h"

#include "ugens/waveform.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Waveform_init(t_Waveform *wave, t_Aleph *aleph) {
    //
    Waveform_init_to_pool(wave, aleph->mempool);
}

void Waveform_init_to_pool(t_Waveform *wave, t_Mempool *mempool) {

    wave = (t_Waveform *)mpool_alloc(sizeof(t_Waveform), mempool);

    wave->freq = 0;
    wave->phase = 0;
    wave->shape = 0;
}

void Waveform_next(t_Waveform *wave) {
    //
}

void Waveform_set_freq(t_Waveform *wave) {
    //
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
