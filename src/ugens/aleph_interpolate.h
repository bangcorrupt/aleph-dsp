/*----------------------------------------------------------------------

                     This file is part of Aleph DSP

                https://github.com/bangcorrupt/aleph-dsp

         Aleph DSP is based on monome/aleph and spiricom/LEAF.

                              MIT License

            Aleph dedicated to the public domain by monome.

                LEAF Copyright Jeff Snyder et. al. 2020

                       Copyright bangcorrupt 2024

----------------------------------------------------------------------*/

/* Original work by monome, modified by bangcorrupt 2024. */

/*
 * @file    aleph_interpolate.h
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

#include "aleph.h"

#include "aleph_utils.h"

/*----- Macros -------------------------------------------------------*/

#define SLEW_1MS (FR32_MAX - (TWOPI * NORMALISED_FREQUENCY(1000)))
#define SLEW_10MS (FR32_MAX - (TWOPI * NORMALISED_FREQUENCY(100)))
#define SLEW_100MS (FR32_MAX - (TWOPI * NORMALISED_FREQUENCY(10)))
#define SLEW_1S (FR32_MAX - (TWOPI * NORMALISED_FREQUENCY(1)))
#define SLEW_4S (FR32_MAX - (1 << 16))

#define SLEW_1MS_16 (FR16_MAX - 1024)
#define SLEW_10MS_16 (FR16_MAX - 180)
#define SLEW_100MS_16 (FR16_MAX - 8)
#define SLEW_1S_16 (FR16_MAX - 1)

#define LINSLEW_1MS (FR32_MAX / 48)
#define LINSLEW_10MS (FR32_MAX / 48 / 10)
#define LINSLEW_100MS (FR32_MAX / 48 / 100)
#define LINSLEW_1S (FR32_MAX / 48 / 1000)
#define LINSLEW_10S (FR32_MAX / 48 / 10000)

#define SIMPLE_SLEW(x, y, slew)                                                \
    (x = add_fr1x32((fract32)(y),                                              \
                    mult_fr1x32x32(sub_fr1x32(FR32_MAX, (fract32)(slew)),      \
                                   sub_fr1x32((fract32)(x), (fract32)(y)))))

#define SIMPLE_SLEW_CHEAT(x, y, slew)                                          \
    (x = add_fr1x32(                                                           \
         (fract32)(y),                                                         \
         mult_fr1x32x32_cheat(sub_fr1x32(FR32_MAX, (fract32)(slew)),           \
                              sub_fr1x32((fract32)(x), (fract32)(y)))))

#define SIMPLE_LPF(x, y, hz) SIMPLE_SLEW(x, y, TWOPI *NORMALISED_FREQUENCY(hz))


/*----- Typedefs -----------------------------------------------------*/

typedef struct {
    uint16_t radix;
    fract32 remainder;
    fract32 rate;
} t_Aleph_RadixLinSlew;

typedef struct {
    fract32 up;
    fract32 down;
} t_Aleph_AsymLinSlew;

typedef struct {
    unsigned short radix;
    fract32 remainder;
    fract32 rate;
} t_Aleph_RadixLogSlew;

typedef struct {
    fract32 rate;
} t_Aleph_LinSlew;

/*----- Extern variable declarations ---------------------------------*/

/*----- Extern function prototypes -----------------------------------*/

void Aleph_RadixLinSlew_init(t_Aleph_RadixLinSlew *slew, fract32 rate,
                             uint16_t radix);

void Aleph_RadixLinSlew_next(t_Aleph_RadixLinSlew *slew, fract32 *current,
                             fract32 target);

void LinSlew_init(t_Aleph_LinSlew *slew, fract32 rate);
void LinSlew_next(t_Aleph_LinSlew *slew, fract32 *current, fract32 target);

void Aleph_AsymLinSlew_init(t_Aleph_AsymLinSlew *slew, fract32 slew_up,
                            fract32 slew_down);

void Aleph_AsymLinSlew_next(t_Aleph_AsymLinSlew *slewm, fract32 *current,
                            fract32 target);

void Aleph_RadixLogSlew_init(t_Aleph_RadixLogSlew *slew, fract32 rate,
                             uint16_t radix);

void Aleph_RadixLogSlew_next(t_Aleph_RadixLogSlew *slew, fract32 *current,
                             fract32 target);

/// TODO: Refactor these as above.
//
void fine_log_slew(fract32 *current, fract32 target, fract32 speed);

void coarse_log_slew(fract32 *current, fract32 target, fract32 speed);

void normalised_log_slew(fract32 *current, fract32 target, fract32 speed);
void normalised_log_slew_16(fract16 *current, fract16 target, fract16 speed);

float interp_bspline_float(float x, float _y, float y, float y_, float y__);

/*----- Static function implementations ------------------------------*/

/// TODO: Is there any actual performance advantage defining this here?

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

#ifdef __cplusplus
}
#endif
#endif

/*----- End of file --------------------------------------------------*/
