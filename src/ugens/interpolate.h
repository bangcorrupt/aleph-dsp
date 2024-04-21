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
 * @file    interpolate.h
 *
 * @brief   Public API for interpolation.
 *
 */

#ifndef ALEPH_INTERPOLATE_H
#define ALEPH_INTERPOLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*----- Includes -----------------------------------------------------*/

#include "fix.h"
#include "types.h"

#include "ricks_tricks.h"

/*----- Macros and Definitions ---------------------------------------*/

#define simple_slew(x, y, slew)                                                \
    (x = add_fr1x32((fract32)(y),                                              \
                    mult_fr1x32x32(sub_fr1x32(FR32_MAX, (fract32)(slew)),      \
                                   sub_fr1x32((fract32)(x), (fract32)(y)))))

#define simple_slew_cheat(x, y, slew)                                          \
    (x = add_fr1x32(                                                           \
         (fract32)(y),                                                         \
         mult_fr1x32x32_cheat(sub_fr1x32(FR32_MAX, (fract32)(slew)),           \
                              sub_fr1x32((fract32)(x), (fract32)(y)))))

#define simple_lpf(x, y, hz) simple_slew(x, y, TWOPI *hzToDimensionless(hz))

#define SLEW_1MS (FR32_MAX - (TWOPI * hzToDimensionless(1000)))
#define SLEW_10MS (FR32_MAX - (TWOPI * hzToDimensionless(100)))
#define SLEW_100MS (FR32_MAX - (TWOPI * hzToDimensionless(10)))
#define SLEW_1S (FR32_MAX - (TWOPI * hzToDimensionless(1)))
#define SLEW_4S (FR32_MAX - (1 << 16))

#define SLEW_1MS_16 (FR16_MAX - 1024)
#define SLEW_10MS_16 (FR16_MAX - 180)
#define SLEW_100MS_16 (FR16_MAX - 8)
#define SLEW_1S_16 (FR16_MAX - 1)

typedef struct {
    unsigned short radix;
    fract32 remainder;
    u32 slewRate;
} radixLinSlew;

typedef struct {
    fract32 up;
    fract32 down;
} asymLinSlew;

typedef struct {
    unsigned short radix;
    fract32 remainder;
    fract32 speed;
} logSlew;

/*----- Extern variable declarations ---------------------------------*/

static const fract32 LINSLEW_1MS = FR32_MAX / 48;
static const fract32 LINSLEW_10MS = FR32_MAX / 48 / 10;
static const fract32 LINSLEW_100MS = FR32_MAX / 48 / 100;
static const fract32 LINSLEW_1S = FR32_MAX / 48 / 1000;
static const fract32 LINSLEW_10S = FR32_MAX / 48 / 10000;

/*----- Static function implementations ------------------------------*/

static inline fract16 interp_bspline_fract16(fract16 x, fract16 _y, fract16 y,
                                             fract16 y_, fract16 y__) {
    fract16 ym1py1 = add_fr1x16(shl_fr1x16(_y, -5), shl_fr1x16(y_, -5));
    fract16 c0 =
        multr_fr1x16(add_fr1x16(ym1py1, shl_fr1x16(y, -3)), FR16_1_OVER_3);
    fract16 c1 = shl_fr1x16(sub_fr1x16(y_, _y), -5);
    fract16 c2 = sub_fr1x16(ym1py1, shl_fr1x16(y, -4));
    fract16 c3 =
        add_fr1x16(shl_fr1x16(y - y_, -5),
                   multr_fr1x16(shl_fr1x16(y__ - _y, -5), FR16_1_OVER_3));
    return shl_fr1x16(
        add_fr1x16(
            c0, multr_fr1x16(
                    x, add_fr1x16(
                           c1, multr_fr1x16((multr_fr1x16(c3, x) + c2), x)))),
        4);
}

static inline fract32 interp_bspline_fract32(fract32 x, fract32 _y, fract32 y,
                                             fract32 y_, fract32 y__) {
    fract32 ym1py1 = add_fr1x32(shl_fr1x32(_y, -5), shl_fr1x32(y_, -5));
    fract32 c0 =
        mult_fr1x32x32(add_fr1x32(ym1py1, shl_fr1x32(y, -3)), FR32_1_OVER_3);
    fract32 c1 = shl_fr1x32(sub_fr1x32(y_, _y), -5);
    fract32 c2 = sub_fr1x32(ym1py1, shl_fr1x32(y, -4));
    fract32 c3 =
        add_fr1x32(shl_fr1x32(y - y_, -5),
                   mult_fr1x32x32(shl_fr1x32(y__ - _y, -5), FR32_1_OVER_3));
    return shl_fr1x32(
        add_fr1x32(
            c0, mult_fr1x32x32(
                    x, add_fr1x32(c1, mult_fr1x32x32(
                                          (mult_fr1x32x32(c3, x) + c2), x)))),
        4);
}

/*----- Extern function prototypes -----------------------------------*/

void radixLinSlew_init(radixLinSlew *slew, unsigned short radix);
void radixLinSlew_next(fract32 *current, fract32 target, radixLinSlew *slew);
void linSlew_next(fract32 *current, fract32 target, fract32 slewRate);
void asymLinSlew_init(asymLinSlew *slew);
void asymLinSlew_next(fract32 *current, fract32 target, asymLinSlew *slew);
void radixLogSlew_init(logSlew *slew, unsigned short radix);
void radixLogSlew_next(fract32 *current, fract32 target, logSlew *slew);
void fine_logSlew(fract32 *current, fract32 target, fract32 speed);
void coarse_logSlew(fract32 *current, fract32 target, fract32 speed);
void normalised_logSlew(fract32 *current, fract32 target, fract32 speed);
void normalised_logSlew_16(fract16 *current, fract16 target, fract16 speed);
float interp_bspline_float(float x, float _y, float y, float y_, float y__);

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
