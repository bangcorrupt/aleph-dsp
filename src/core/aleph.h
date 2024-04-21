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

#include "aleph-mempool.h"

#include "fix.h"
#include "fract_math.h"
#include "types.h"


/*----- Macros and Definitions ---------------------------------------*/

#define TWO_PI 6

struct Aleph {
    ///@{
    fract32 samplerate;           //!< The current audio sample rate. Set with
                                  //!< Aleph_set_samplerate().
    fract32 inv_samplerate;       //!< The inverse of the current sample rate.
    int block_size;               //!< The audio block size.
    fract32 twopi_inv_samplerate; //!<  Two-pi times the inverse of the
                                  //!<  current sample rate.
    fract32 (*random)(void); //!< A pointer to the random() function provided on
                             //!< initialization.
    bool clear_on_alloc;     //!< A flag that determines whether memory
                             //!< allocated from the Aleph memory pool will be
                             //!< cleared.
    t_Mempool mempool;       //!< The default Aleph mempool object.
    _t_Mempool _internal_mempool;
    size_t header_size; //!< The size in bytes of memory region headers within
                        //!< mempools.
    void (*error_callback)(
        Aleph *const,
        e_Aleph_error_type); //!< A pointer to the callback function for
                             //!< Aleph errors. Can be set by the user.
    int error_state[ALEPH_ERROR_NIL]; //!< An array of flags that indicate which
                                      //!< errors have occurred.
    unsigned int alloc_count;         //!< A count of Aleph memory allocations.
    unsigned int free_count;          //!< A count of Aleph memory frees.
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
void Aleph_init(Aleph *const leaf, fract32 samplerate, char *memory,
                size_t memory_size, fract32 (*random)(void));

//! Set the sample rate of Aleph.
/*!
 @param samplerate The new audio sample rate.
 */
void Aleph_set_samplerate(Aleph *const leaf, fract32 samplerate);

//! Get the sample rate of Aleph.
/*!
 @return The current sample rate as a fract32.
 */
fract32 Aleph_get_samplerate(Aleph *const leaf);

//! The default callback function for Aleph errors.
/*!
 @param error_type The type of the error that has occurred.
 */
void Aleph_default_error_callback(Aleph *const leaf,
                                  e_Aleph_error_type error_type);

void Aleph_internal_error_callback(Aleph *const leaf,
                                   e_Aleph_error_type whichone);

//! Set the callback function for Aleph errors.
/*!
 @param callback A pointer to the callback function.
 */
void Aleph_set_error_callback(Aleph *const leaf,
                              void (*callback)(Aleph *const,
                                               e_Aleph_error_type));

/*! @} */

#ifdef __cplusplus
}
#endif
#endif // ALEPH_H

/*----- End of file --------------------------------------------------*/
