/**
 * @file include/ld36/type.h
 *
 * Define types for components (useful when colliding stuff)
 */
#ifndef __TYPE_H__
#define __TYPE_H__

#include <GFraMe/gfmTypes.h>

/**
 * Insteresting feature about types and the quadtree:
 *
 * If a type 'T' is greater or equal to gfmType_max, it will be displayed with
 * the same color as the type '(T % gfmType_max) + 1'.
 *
 * So, it's possible to have to different enemies be displayed with the same
 * color, even though they are handled differently by the code during collisiong
 */

/* Mask that returns only the entity, ignoring its base type */
#define T_MASK 0x0000ffe0
/* Number of bits for the types */
#define T_NBITS 5

enum enType {
    T_LENS = gfmType_reserved_2, /* pink */
    T_LIGHT = gfmType_reserved_4, /* green */
    T_FLOOR = gfmType_reserved_5, /* purple */
    T_TORCH  = gfmType_reserved_6, /* yellow */
    T_PLAYER = gfmType_reserved_7, /* light red */
    T_SOURCE = gfmType_reserved_11,  /* white */


    T_TORCH_HEAD     = (1 << T_NBITS) | T_TORCH,
    T_TORCH_HEAD_LIT = (2 << T_NBITS) | T_TORCH,
    T_TORCH_BOOT     = (3 << T_NBITS) | T_TORCH,
    T_TORCH_BOOT_LIT = (4 << T_NBITS) | T_TORCH,
    T_TORCH_HAND     = (5 << T_NBITS) | T_TORCH,
    T_TORCH_HAND_LIT = (6 << T_NBITS) | T_TORCH
};
typedef enum enType type;

#endif /* __TYPE_H__ */

