/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

----------------------------------------------------------------------*/

/* Modified by bangcorrupt 2024. */

/** mpool source significantly modified by Mike Mulshine, Jeff Snyder, et al.,
 * Princeton University Music Department **/

/**
 In short, mpool is distributed under so called "BSD license",

 Copyright (c) 2009-2010 Tatsuhiko Kubo <cubicdaiya@gmail.com>
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of the authors nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* written with C99 style */

/*----- Includes -----------------------------------------------------*/

#ifndef ALEPH_MPOOL_H
#define ALEPH_MPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==============================================================================

#define MPOOL_ALIGN_SIZE (16)

/// TODO: Find a better place for this.
//
typedef struct t_Aleph t_Aleph;

typedef enum {
    ALEPH_MEMPOOL_OVERRUN = 0,
    ALEPH_MEMPOOL_FRAGMENTATION,
    ALEPH_INVALID_FREE,
    ALEPH_ERROR_NIL
} e_Aleph_error_type;

/*!
 * @defgroup tmempool t_Mempool
 * @ingroup mempool
 * @brief Memory pool for the allocation of Aleph objects.
 * @{
 */

// node of free list
typedef struct t_mpool_node {
    char *pool;                // memory pool field
    struct t_mpool_node *next; // next node pointer
    struct t_mpool_node *prev; // prev node pointer
    size_t size;
} t_mpool_node;

typedef struct t_Mempool t_Mempool;

struct t_Mempool {
    /// TODO: See aleph-mempool.c:Mempool_free()
    //
    t_Mempool *mempool;
    t_Aleph *aleph;
    char *mpool;        // start of the mpool
    size_t usize;       // used size of the pool
    size_t msize;       // max size of the pool
    t_mpool_node *head; // first node of memory pool free list
};

//! Initialize a t_Mempool for a given memory location and size to the default
//! mempool of a Aleph instance.
/*!
 @param pool A pointer to the t_Mempool to initialize.
 @param memory A pointer to the chunk of memory to be used as a mempool.
 @param size The size of the chunk of memory to be used as a mempool.
 @param aleph A pointer to the aleph instance.
 */
void Mempool_init(t_Mempool *const pool, char *memory, size_t size,
                  t_Aleph *const aleph);

//! Free a t_Mempool from its mempool.
/*!
 @param pool A pointer to the t_Mempool to free.
 */
void Mempool_free(t_Mempool *const pool);

//! Initialize a t_Mempool for a given memory location and size to a specified
//! mempool.
/*!
 @param pool A pointer to the t_Mempool to initialize.
 @param memory A pointer to the chunk of memory to be used as a mempool.
 @param size The size of the chuck of memory to be used as a mempool.
 @param pool_to A pointer to the t_Mempool to which this t_Mempool should be
 initialized.
 */
void Mempool_init_to_pool(t_Mempool *const mp, char *memory, size_t size,
                          t_Mempool *const mem);

/*!￼￼￼
 @} */

//==============================================================================

//    typedef struct mpool_t {
//        char*         mpool;       // start of the mpool
//        size_t        usize;       // used size of the pool
//        size_t        msize;       // max size of the pool
//        t_mpool_node* head;        // first node of memory pool free list
//    } mpool_t;

void mpool_create(char *memory, size_t size, t_Mempool *pool);

char *mpool_alloc(size_t size, t_Mempool *pool);
char *mpool_calloc(size_t asize, t_Mempool *pool);

void mpool_free(char *ptr, t_Mempool *pool);

size_t mpool_get_size(t_Mempool *pool);
size_t mpool_get_used(t_Mempool *pool);

void aleph_pool_init(t_Aleph *const aleph, char *memory, size_t size);

char *aleph_alloc(t_Aleph *const aleph, size_t size);
char *aleph_calloc(t_Aleph *const aleph, size_t size);

void aleph_free(t_Aleph *const aleph, char *ptr);

size_t aleph_pool_get_size(t_Aleph *const aleph);
size_t aleph_pool_get_used(t_Aleph *const aleph);

char *aleph_pool_get_pool(t_Aleph *const aleph);

#ifdef __cplusplus
}
#endif

#endif // ALEPH_MPOOL_H

//==============================================================================
