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
 * @file    aleph-test.c
 *
 * @brief   Tests for Aleph DSP.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"
#include "filter_svf.h"

/*----- Macros and Definitions ---------------------------------------*/

#define SAMPLERATE 48000
#define MEMPOOL_SIZE 0x4000

/*----- Static variable definitions ----------------------------------*/

static Aleph *g_aleph;

static char g_mempool[MEMPOOL_SIZE];

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

int main(void) {

    t_FilterSVF *svf;

    Aleph_init(g_aleph, SAMPLERATE, g_mempool, MEMPOOL_SIZE, NULL);

    FilterSVF_init(svf, g_aleph);

    return 0;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
