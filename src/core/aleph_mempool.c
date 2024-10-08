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

/**
 * @file    aleph_mempool.c
 *
 * @brief   Mempool implementation for Aleph DSP.
 */

/*----- Includes -----------------------------------------------------*/

#include <stdlib.h>

#include "aleph.h"

#include "aleph_mempool.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

static inline size_t mpool_align(size_t size);
static inline t_mpool_node *create_node(char *block_location,
                                        t_mpool_node *next, t_mpool_node *prev,
                                        size_t size, size_t header_size);
static inline void delink_node(t_mpool_node *node);

/*----- Extern function implementations ------------------------------*/

void Mempool_init(Mempool *mp, char *memory, size_t size, t_Aleph *aleph) {

    Mempool_init_to_pool(mp, memory, size, &aleph->mempool);
}

void Mempool_free(Mempool *const mp) {

    t_Mempool *m = *mp;

    mpool_free((char *)m, m->mempool);
}

void Mempool_init_to_pool(Mempool *const mp, char *memory, size_t size,
                          Mempool *const mem) {

    t_Mempool *mm = *mem;

    t_Mempool *m = *mp = (t_Mempool *)mpool_alloc(sizeof(t_Mempool), mm);

    m->aleph = mm->aleph;

    mpool_create(memory, size, m);
}

void aleph_pool_init(t_Aleph *aleph, char *memory, size_t size) {

    mpool_create(memory, size, &aleph->_internal_mempool);

    aleph->mempool = &aleph->_internal_mempool;
}

char *aleph_alloc(t_Aleph *const aleph, size_t size) {
    // printf("alloc %i\n", size);
    return mpool_alloc(size, &aleph->_internal_mempool);
}

char *aleph_calloc(t_Aleph *const aleph, size_t size) {
    // printf("alloc %i\n", size);
    return mpool_calloc(size, &aleph->_internal_mempool);
}

void aleph_free(t_Aleph *const aleph, char *ptr) {
    mpool_free(ptr, &aleph->_internal_mempool);
}

size_t aleph_pool_get_size(t_Aleph *const aleph) {
    return mpool_get_size(&aleph->_internal_mempool);
}

size_t aleph_pool_get_used(t_Aleph *const aleph) {
    return mpool_get_used(&aleph->_internal_mempool);
}

char *aleph_pool_get_pool(t_Aleph *const aleph) {
    char *buff = aleph->_internal_mempool.mpool;

    return buff;
}

/**
 * create memory pool
 */
void mpool_create(char *memory, size_t size, t_Mempool *pool) {

    pool->aleph->header_size = mpool_align(sizeof(t_mpool_node));

    pool->mpool = (char *)memory;
    pool->usize = 0;
    if (size < pool->aleph->header_size) {
        size = pool->aleph->header_size;
    }
    pool->msize = size;

    pool->head = create_node(pool->mpool, NULL, NULL,
                             pool->msize - pool->aleph->header_size,
                             pool->aleph->header_size);
}

/**
 * allocate memory from memory pool
 */
char *mpool_alloc(size_t asize, t_Mempool *pool) {
    pool->aleph->alloc_count++;
#if Aleph_DEBUG
    DBG("alloc " + String(asize));
#endif
#if Aleph_USE_DYNAMIC_ALLOCATION
    char *temp = (char *)malloc(asize);
    if (temp == NULL) {
        // allocation failed, exit from the program
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    if (pool->aleph->clear_on_allocation > 0) {
        memset(temp, 0, asize);
    }
    return temp;
#else
    // If the head is NULL, the mempool is full
    if (pool->head == NULL) {
        if ((pool->msize - pool->usize) > asize) {
            Aleph_internal_error_callback(pool->aleph,
                                          ALEPH_MEMPOOL_FRAGMENTATION);
        } else {
            Aleph_internal_error_callback(pool->aleph, ALEPH_MEMPOOL_OVERRUN);
        }
        return NULL;
    }

    // Should we alloc the first block large enough or check all blocks and pick
    // the one closest in size?
    size_t size_to_alloc = mpool_align(asize);
    t_mpool_node *node_to_alloc = pool->head;

    // Traverse the free list for a large enough block
    while (node_to_alloc->size < size_to_alloc) {
        node_to_alloc = node_to_alloc->next;

        // If we reach the end of the free list, there
        // are no blocks large enough, return NULL
        if (node_to_alloc == NULL) {
            if ((pool->msize - pool->usize) > asize) {
                Aleph_internal_error_callback(pool->aleph,
                                              ALEPH_MEMPOOL_FRAGMENTATION);
            } else {
                Aleph_internal_error_callback(pool->aleph,
                                              ALEPH_MEMPOOL_OVERRUN);
            }
            return NULL;
        }
    }

    // Create a new node after the node to be allocated if there is enough space
    t_mpool_node *new_node;
    size_t leftover = node_to_alloc->size - size_to_alloc;

    node_to_alloc->size = size_to_alloc;

    if (leftover > pool->aleph->header_size) {

        long offset = (char *)node_to_alloc - (char *)pool->mpool;
        offset += pool->aleph->header_size + node_to_alloc->size;
        new_node = create_node(
            &pool->mpool[offset], node_to_alloc->next, node_to_alloc->prev,
            leftover - pool->aleph->header_size, pool->aleph->header_size);

    } else {
        // Add any leftover space to the allocated node to avoid fragmentation
        node_to_alloc->size += leftover;
        new_node = node_to_alloc->next;
    }

    // Update the head if we are allocating the first node of the free list
    // The head will be NULL if there is no space left
    if (pool->head == node_to_alloc) {
        pool->head = new_node;
    }

    // Remove the allocated node from the free list
    delink_node(node_to_alloc);

    pool->usize += pool->aleph->header_size + node_to_alloc->size;

    int i;
    if (pool->aleph->clear_on_alloc > 0) {

        char *new_pool = (char *)node_to_alloc->pool;

        for (i = 0; i < node_to_alloc->size; i++)
            new_pool[i] = 0;
    }

    // Return the pool of the allocated node;
    return node_to_alloc->pool;
#endif
}

/**
 * allocate memory from memory pool and also clear that memory to be blank
 */
char *mpool_calloc(size_t asize, t_Mempool *pool) {
    pool->aleph->alloc_count++;
#if Aleph_DEBUG
    DBG("calloc " + String(asize));
#endif
#if Aleph_USE_DYNAMIC_ALLOCATION
    char *ret = (char *)malloc(asize);
    if (ret == NULL) {
        // allocation failed, exit from the program
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    memset(ret, 0, asize);
    return ret;
#else
    // If the head is NULL, the mempool is full
    if (pool->head == NULL) {
        if ((pool->msize - pool->usize) > asize) {
            Aleph_internal_error_callback(pool->aleph,
                                          ALEPH_MEMPOOL_FRAGMENTATION);
        } else {
            Aleph_internal_error_callback(pool->aleph, ALEPH_MEMPOOL_OVERRUN);
        }

        return NULL;
    }

    // Should we alloc the first block large enough or check all blocks and pick
    // the one closest in size?
    size_t size_to_alloc = mpool_align(asize);
    t_mpool_node *node_to_alloc = pool->head;

    // Traverse the free list for a large enough block
    while (node_to_alloc->size < size_to_alloc) {
        node_to_alloc = node_to_alloc->next;

        // If we reach the end of the free list, there
        // are no blocks large enough, return NULL
        if (node_to_alloc == NULL) {
            if ((pool->msize - pool->usize) > asize) {
                Aleph_internal_error_callback(pool->aleph,
                                              ALEPH_MEMPOOL_FRAGMENTATION);
            } else {
                Aleph_internal_error_callback(pool->aleph,
                                              ALEPH_MEMPOOL_OVERRUN);
            }

            return NULL;
        }
    }

    // Create a new node after the node to be allocated if there is enough space
    t_mpool_node *new_node;
    size_t leftover = node_to_alloc->size - size_to_alloc;

    node_to_alloc->size = size_to_alloc;

    if (leftover > pool->aleph->header_size) {

        long offset = (char *)node_to_alloc - (char *)pool->mpool;
        offset += pool->aleph->header_size + node_to_alloc->size;

        new_node = create_node(
            &pool->mpool[offset], node_to_alloc->next, node_to_alloc->prev,
            leftover - pool->aleph->header_size, pool->aleph->header_size);

    } else {
        // Add any leftover space to the allocated node to avoid fragmentation
        node_to_alloc->size += leftover;
        new_node = node_to_alloc->next;
    }

    // Update the head if we are allocating the first node of the free list
    // The head will be NULL if there is no space left
    if (pool->head == node_to_alloc) {
        pool->head = new_node;
    }

    // Remove the allocated node from the free list
    delink_node(node_to_alloc);

    pool->usize += pool->aleph->header_size + node_to_alloc->size;
    // Format the new pool
    int i;
    for (i = 0; i < node_to_alloc->size; i++) {
        node_to_alloc->pool[i] = 0;
    }

    // Return the pool of the allocated node;
    return node_to_alloc->pool;

#endif
}

void mpool_free(char *ptr, t_Mempool *pool) {
    pool->aleph->free_count++;
#if Aleph_DEBUG
    DBG("free");
#endif
#if Aleph_USE_DYNAMIC_ALLOCATION
    free(ptr);
#else
    // if (ptr < pool->mpool || ptr >= pool->mpool + pool->msize)
    //  Get the node at the freed space
    t_mpool_node *freed_node = (t_mpool_node *)(ptr - pool->aleph->header_size);

    pool->usize -= pool->aleph->header_size + freed_node->size;

    // Check each node in the list against the newly freed one to see if it's
    // adjacent in memory
    t_mpool_node *other_node = pool->head;
    t_mpool_node *next_node;

    while (other_node != NULL) {

        if ((long)other_node < (long)pool->mpool ||
            (long)other_node >= (((long)pool->mpool) + pool->msize)) {

            Aleph_internal_error_callback(pool->aleph, ALEPH_INVALID_FREE);
            return;
        }

        next_node = other_node->next;
        // Check if a node is directly after the freed node
        if (((long)freed_node) +
                (pool->aleph->header_size + freed_node->size) ==
            (long)other_node) {

            // Increase freed node's size
            freed_node->size += pool->aleph->header_size + other_node->size;

            // If we are merging with the head, move the head forward
            if (other_node == pool->head) {
                pool->head = pool->head->next;
            }

            // Delink the merged node
            delink_node(other_node);
            // Check if a node is directly before the freed node
        } else if (((long)other_node) +
                       (pool->aleph->header_size + other_node->size) ==
                   (long)freed_node) {

            // Increase the merging node's size
            other_node->size += pool->aleph->header_size + freed_node->size;

            if (other_node != pool->head) {
                // Delink the merging node
                delink_node(other_node);
                // Attach the merging node to the head
                other_node->next = pool->head;
                // Merge
                freed_node = other_node;

            } else {
                // If we are merging with the head, move the head forward
                pool->head = pool->head->next;
                // Merge
                freed_node = other_node;
            }
        }

        other_node = next_node;
    }

    // Ensure the freed node is attached to the head
    freed_node->next = pool->head;

    if (pool->head != NULL) {
        pool->head->prev = freed_node;
    }

    pool->head = freed_node;

    // Format the freed pool
    //    char* freed_pool = (char*)freed_node->pool;
    //    for (int i = 0; i < freed_node->size; i++) freed_pool[i] = 0;
#endif
}

size_t mpool_get_size(t_Mempool *pool) { return pool->msize; }

size_t mpool_get_used(t_Mempool *pool) { return pool->usize; }

/*----- Static function implementations ------------------------------*/

/**
 * align byte boundary
 */
static inline size_t mpool_align(size_t size) {

    return (size + (MPOOL_ALIGN_SIZE - 1)) & ~(MPOOL_ALIGN_SIZE - 1);
}

static inline t_mpool_node *create_node(char *block_location,
                                        t_mpool_node *next, t_mpool_node *prev,
                                        size_t size, size_t header_size) {

    t_mpool_node *node = (t_mpool_node *)block_location;
    node->pool = block_location + header_size;
    node->next = next;
    node->prev = prev;
    node->size = size;

    return node;
}

static inline void delink_node(t_mpool_node *node) {
    // If there is a node after the node to remove
    if (node->next != NULL) {
        // Close the link
        node->next->prev = node->prev;
    }
    // If there is a node before the node to remove
    if (node->prev != NULL) {
        // Close the link
        node->prev->next = node->next;
    }

    node->next = NULL;
    node->prev = NULL;
}

/*----- End of file --------------------------------------------------*/
