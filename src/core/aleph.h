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

/*
 * @file    aleph.h
 *
 * @brief   Public API for Aleph DSP.
 *
 */

#ifndef ALEPH_H
#define ALEPH_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>

#include "aleph_mempool.h"

#include "fix.h"
#include "fix16_fract.h"
#include "fract_math.h"
#include "types.h"

/*----- Macros -------------------------------------------------------*/

#define TWO_PI (6)
#define ALEPH_MAX_UNISON_VOICES (16)

struct t_Aleph {
    ///@{
    uint32_t samplerate; //!< The current audio sample rate. Set with
                         //!< Aleph_set_samplerate().
    uint32_t block_size; //!< The audio block size.

    /// TODO: Calculate fract32 values.
    //
    /* fract32 inv_samplerate;       //!< The inverse of the current sample
     * rate. */
    /* fract32 twopi_inv_samplerate; //!<  Two-pi times the inverse of the */
    //!<  current sample rate.

    fract32 (*random)(void); //!< A pointer to the random() function provided on
                             //!< initialization.
    bool clear_on_alloc;     //!< A flag that determines whether memory
                             //!< allocated from the Aleph memory pool will be
                             //!< cleared.
    t_Mempool *mempool;      //!< The default Aleph mempool object.
    t_Mempool _internal_mempool;
    size_t header_size; //!< The size in bytes of memory region headers within
                        //!< mempools.
    void (*error_callback)(
        t_Aleph *const,
        e_Aleph_error_type); //!< A pointer to the callback function for
                             //!< Aleph errors. Can be set by the user.
    int error_state[ALEPH_ERROR_NIL]; //!< An array of flags that indicate which
                                      //!< errors have occurred.
    uint32_t alloc_count;             //!< A count of Aleph memory allocations.
    uint32_t free_count;              //!< A count of Aleph memory frees.
                                      ///@}
};

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

//! Initialize the Aleph instance.
/*!
 @param samplerate The default sample rate for object initialized to this Aleph
 instance.
 @param memory A pointer to the memory that will make up the default mempool of
 a Aleph instance.
 @param memory_size The size of the memory that will make up the default mempool
 of a Aleph instance.
 @param random A pointer to a random number function. Should return a fract32 >=
 0 and < 1.
 */
void Aleph_init(t_Aleph *const aleph, uint32_t samplerate, char *memory,
                size_t memory_size, fract32 (*random)(void));

//! Set the sample rate of Aleph.
/*!
 @param samplerate The new audio sample rate.
 */
void Aleph_set_samplerate(t_Aleph *const aleph, uint32_t samplerate);

//! Get the sample rate of Aleph.
/*!
 @return The current sample rate as a fract32.
 */
fract32 Aleph_get_samplerate(t_Aleph *const aleph);

//! The default callback function for Aleph errors.
/*!
 @param error_type The type of the error that has occurred.
 */
void Aleph_default_error_callback(t_Aleph *const aleph,
                                  e_Aleph_error_type error_type);

void Aleph_internal_error_callback(t_Aleph *const aleph,
                                   e_Aleph_error_type whichone);

//! Set the callback function for Aleph errors.
/*!
 @param callback A pointer to the callback function.
 */
void Aleph_set_error_callback(t_Aleph *const aleph,
                              void (*callback)(t_Aleph *const,
                                               e_Aleph_error_type));

// Return pointer to Aleph mempool.
t_Mempool *Aleph_get_mempool(t_Aleph *const aleph);

/*! @} */

#ifdef __cplusplus
}
#endif
#endif // ALEPH_H

/*----- End of file --------------------------------------------------*/
