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

/**
 * @file    aleph_interpolate.c
 *
 * @brief   Iterpolation.
 *
 */

/*----- Includes -----------------------------------------------------*/

#include <stdint.h>

#include "aleph_interpolate.h"

/*----- Macros -------------------------------------------------------*/

/*----- Typedefs -----------------------------------------------------*/

/*----- Static variable definitions ----------------------------------*/

/*----- Extern variable definitions ----------------------------------*/

/*----- Static function prototypes -----------------------------------*/

/*----- Extern function implementations ------------------------------*/

/*----- Static function implementations ------------------------------*/

// 4-point, 3rd-order B-spline (x-form) from
// http://yehar.com/blog/wp-content/uploads/2009/08/deip.pdf
float interp_bspline_float(float x, float _y, float y, float y_, float y__) {

    float ym1py1 = _y + y_;
    float c0 = 1 / 6.0 * ym1py1 + 2 / 3.0 * y;
    float c1 = 1 / 2.0 * (y_ - _y);
    float c2 = 1 / 2.0 * ym1py1 - y;
    float c3 = 1 / 2.0 * (y - y_) + 1 / 6.0 * (y__ - _y);
    return ((c3 * x + c2) * x + c1) * x + c0;
}

void Aleph_RadixLinSlew_init(t_Aleph_RadixLinSlew *slew, fract32 rate,
                             uint16_t radix) {

    slew->radix = radix;
    slew->remainder = 0;
    slew->rate = rate; // Previously  LINSLEW_100MS.
}

// This guy slews between target and current at *fractional* constant speed
void Aleph_RadixLinSlew_next(t_Aleph_RadixLinSlew *slew, fract32 *current,
                             fract32 target) {

    fract32 difference = abs_fr1x32(sub_fr1x32(target, *current));

    fract32 fract_slew =
        min_fr1x32(shl_fr1x32(difference, slew->radix), slew->rate);
    fract_slew += slew->remainder;

    uint32_t remainder_mask = ~(0xFFFFFFFF << slew->radix);
    slew->remainder = remainder_mask & fract_slew;

    uint32_t inc = shl_fr1x32(fract_slew, -slew->radix);
    if (*current > target)
        *current -= inc;
    else if (*current < target)
        *current += inc;
}

void LinSlew_init(t_Aleph_LinSlew *slew, fract32 rate) {
    //
    slew->rate = rate;
}

// This guy slews between target and current at constant speed
void LinSlew_next(t_Aleph_LinSlew *slew, fract32 *current, fract32 target) {

    if (*current > target)
        *current -= min_fr1x32(abs_fr1x32(target - *current), slew->rate);
    else if (*current < target)
        *current += min_fr1x32(abs_fr1x32(target - *current), slew->rate);
}

void Aleph_AsymLinSlew_init(t_Aleph_AsymLinSlew *slew, fract32 slew_up,
                            fract32 slew_down) {

    slew->up = slew_up;     //  Previously  LINSLEW_10MS.
    slew->down = slew_down; //  Previously  LINSLEW_100MS.
}

// This guy slews between target and current at constant speed
void Aleph_AsymLinSlew_next(t_Aleph_AsymLinSlew *slew, fract32 *current,
                            fract32 target) {

    if (*current > target)
        *current -= min_fr1x32(abs_fr1x32(target - *current), slew->down);
    else if (*current < target)
        *current += min_fr1x32(abs_fr1x32(target - *current), slew->up);
}

void Aleph_RadixLogSlew_init(t_Aleph_RadixLogSlew *slew, fract32 rate,
                             uint16_t radix) {

    slew->radix = radix;
    slew->remainder = 0;
    slew->rate = rate; // Previously SLEW_1S;
}

// This guy slews correctly when the slew rate is less than 1 per audio frame
void Aleph_RadixLogSlew_next(t_Aleph_RadixLogSlew *slew, fract32 *current,
                             fract32 target) {

    fract32 ratio = slew->rate;
    fract32 difference = abs_fr1x32(sub_fr1x32(target, *current));
    fract32 fract_slew =
        mult_fr1x32x32(shl_fr1x32(difference, slew->radix), ratio);

    fract_slew = add_fr1x32(fract_slew, slew->remainder);

    uint32_t remainderMask = ~(0xFFFFFFFF << slew->radix);
    slew->remainder = remainderMask & fract_slew;
    uint32_t inc = shl_fr1x32(fract_slew, -slew->radix);
    if (target > *current)
        *current = add_fr1x32(*current, inc);
    else if (target < *current)
        *current = sub_fr1x32(*current, inc);
}

// This guy slews correctly between *small* parameters
void fine_log_slew(fract32 *current, fract32 target, fract32 speed) {

    fract32 ratio = FR32_MAX - speed;
    fract32 difference = sub_fr1x32(*current, target);
    fract32 inc = mult_fr1x32x32(ratio, shl_fr1x32(difference, 12));
    *current = add_fr1x32(target, shl_fr1x32(inc, -12));
}

// This guy slews correctly between *large* parameters
void coarse_log_slew(fract32 *current, fract32 target, fract32 speed) {

    fract32 ratio = FR32_MAX - speed;
    fract32 inc = mult_fr1x32x32(ratio, sub_fr1x32(*current, target));
    *current = add_fr1x32(target, inc);
}

// This guy auto-adjusts the radix - poor man's float I guess...
void normalised_log_slew(fract32 *current, fract32 target, fract32 speed) {

    fract32 ratio = FR32_MAX - speed;
    fract32 difference = sub_fr1x32(target, *current);
    int radix = norm_fr1x32(difference);
    fract32 inc = mult_fr1x32x32(ratio, shl_fr1x32(difference, radix));
    *current = add_fr1x32(*current, shl_fr1x32(inc, -radix));
}

void normalised_log_slew_16(fract16 *current, fract16 target, fract16 speed) {

    fract16 ratio = FR32_MAX - speed;
    fract16 difference = sub_fr1x16(target, *current);
    int radix = norm_fr1x16(difference);
    fract16 inc = mult_fr1x16(ratio, shl_fr1x16(difference, radix));
    *current = add_fr1x16(*current, shr_fr1x16(inc, radix));
}

/*----- End of file --------------------------------------------------*/
