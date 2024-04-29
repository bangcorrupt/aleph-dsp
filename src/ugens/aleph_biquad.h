/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/Aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by monome, modified by bangcorrupt 2024. */

/*
 * @file    biquad.h
 *
 * @brief   Public API for biquad filters.
 *
 */

#ifndef ALEPH_BIQUAD_H
#define ALEPH_BIQUAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "types.h"

/*----- Macros and Definitions ---------------------------------------*/

#define FR8_24_UNITY (1 << 24)

#define mult_fr8x24(a, b) (mult_fr1x32x32(a, b) << 7)
#define mult_fr7_24(a, b) (shl_fr1x32(mult_fr1x32x32(a, b), 7))
/* #define mult_fr15_16(a,b) (shl_fr1x32(mult_fr1x32x32(a,b), 15)) */

#define float_to_radix32(my_float, my_radix)                                   \
    ((fract32)((((float)my_float) * ((float)FR32_MAX)) /                       \
               ((float)pow(2, my_radix))))

#define float_c8x24(my_float) float_to_radix32(my_float, 7)

typedef struct {
    fract32 a1;
    fract32 a2;
    fract32 b0;
    fract32 b1;
    fract32 b2;
    fract32 y_;
    fract32 y__;
    fract32 x_;
    fract32 x__;
} t_Biquad;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Biquad_init(t_Biquad *bq);

int Biquad_next(t_Biquad *bq, fract32 x);

void Biquad_set_lpf(t_Biquad *bq, float f, float q);

void Biquad_set_coeffs_from_floats(t_Biquad *bq, float a1, float a2, float b0,
                                   float b1, float b2);
#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
