/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by Michael R. Mulshine, modified by bangcorrupt 2024. */

/**
 * @file    aleph.c
 *
 * @brief   Aleph DSP Library.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include "aleph.h"
#include <stdint.h>

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

void Aleph_init(t_Aleph *const aleph, uint32_t samplerate, char *memory,
                size_t memory_size, fract32 (*random)(void)) {

    aleph->_internal_mempool.aleph = aleph;

    aleph_pool_init(aleph, memory, memory_size);

    aleph->samplerate = samplerate;

    // aleph->inv_samplerate = 1.0f / samplerate;
    //
    // aleph->twopi_inv_samplerate = aleph->inv_samplerate * TWO_PI;

    aleph->random = random;

    aleph->clear_on_alloc = 0;

    aleph->error_callback = &Aleph_default_error_callback;

    int i;
    for (i = 0; i < ALEPH_ERROR_NIL; ++i)
        aleph->error_state[i] = 0;

    aleph->alloc_count = 0;

    aleph->free_count = 0;
}

void Aleph_set_samplerate(t_Aleph *const aleph, uint32_t samplerate) {
    aleph->samplerate = samplerate;
    // aleph->inv_samplerate = 1.0f / samplerate;
    // aleph->twopi_inv_samplerate = aleph->inv_samplerate * TWO_PI;
}

fract32 Aleph_get_samplerate(t_Aleph *const aleph) { return aleph->samplerate; }

void Aleph_default_error_callback(t_Aleph *const aleph,
                                  e_Aleph_error_type whichone) {
    // Not sure what this should do if anything
    // Maybe fine as a placeholder
}

void Aleph_internal_error_callback(t_Aleph *const aleph,
                                   e_Aleph_error_type whichone) {
    aleph->error_state[whichone] = 1;
    aleph->error_callback(aleph, whichone);
}

void Aleph_set_error_callback(t_Aleph *const aleph,
                              void (*callback)(t_Aleph *const,
                                               e_Aleph_error_type)) {
    aleph->error_callback = callback;
}

// Return pointer to Aleph mempool.
t_Mempool *Aleph_get_mempool(t_Aleph *const aleph) { return aleph->mempool; }

/*----- Static function implementations ------------------------------*/

/*----- End of file --------------------------------------------------*/
