/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/Aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by Michael R. Mulshine, modified by bangcorrupt 2024. */

/*
 * @file    aleph.c
 *
 * @brief   Aleph DSP Library.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"

/*----- Macros and Definitions ---------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_init(Aleph *const aleph, fract32 sr, char *memory, size_t memorysize,
                fract32 (*random)(void)) {
    aleph->_internal_mempool.aleph = aleph;
    aleph_pool_init(aleph, memory, memorysize);

    aleph->samplerate = sr;

    aleph->inv_samplerate = 1.0f / sr;

    aleph->twopi_inv_samplerate = aleph->inv_samplerate * TWO_PI;

    aleph->random = random;

    aleph->clear_on_alloc = 0;

    aleph->error_callback = &Aleph_default_error_callback;

    int i;
    for (i = 0; i < ALEPH_ERROR_NIL; ++i)
        aleph->error_state[i] = 0;

    aleph->alloc_count = 0;

    aleph->free_count = 0;
}

void Aleph_set_samplerate(Aleph *const aleph, fract32 samplerate) {
    aleph->samplerate = samplerate;
    aleph->inv_samplerate = 1.0f / samplerate;
    aleph->twopi_inv_samplerate = aleph->inv_samplerate * TWO_PI;
}

fract32 Aleph_get_samplerate(Aleph *const aleph) { return aleph->samplerate; }

void Aleph_default_error_callback(Aleph *const aleph,
                                  e_Aleph_error_type whichone) {
    // Not sure what this should do if anything
    // Maybe fine as a placeholder
}

void Aleph_internal_error_callback(Aleph *const aleph,
                                   e_Aleph_error_type whichone) {
    aleph->error_state[whichone] = 1;
    aleph->error_callback(aleph, whichone);
}

void Aleph_set_error_callback(Aleph *const aleph,
                              void (*callback)(Aleph *const,
                                               e_Aleph_error_type)) {
    aleph->error_callback = callback;
}

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
