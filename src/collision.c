/**
 * Handle collisions
 *
 * @file src/collision.c
 */
#include <base/collision.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmTypes.h>

#include <ld36/lens.h>
#include <ld36/player.h>
#include <ld36/torch.h>
#include <ld36/type.h>

#if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
#include <stdlib.h>
#  include <signal.h>
#endif

/**
 * Retrieve a object's main subtype
 *
 * @param  [out]ppObj The object's pointer
 * @param  [out]pType The object's type
 * @param  [ in]pObj  The gfmObject being collided/overlaped
 * @param             GFraMe return value
 */
static inline gfmRV collision_getSubtype(void **ppObj, int *pType, gfmObject *pObj) {
    /** GFraMe return value */
    gfmRV rv;

    /** Get the object's child, if any (it's usually a sprite, but tilemaps, for
     * examples, doesn't  have a child even though they have a type (which is
     * OK) */
    rv = gfmObject_getChild(ppObj, pType, pObj);
    ASSERT(rv == GFMRV_OK, rv);

    if (*pType == gfmType_sprite) {
        /** The object's child, a sprite */
        gfmSprite *pSpr;

        /** Retrieve the child of the sprite */
        pSpr = *((gfmSprite**)ppObj);
        rv = gfmSprite_getChild(ppObj, pType, pSpr);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Continue the currently executing collision and handle the interaction of
 * objects
 *
 * @return GFraMe return value
 */
gfmRV collision_run() {
    /** GFraMe return value */
    gfmRV rv;

    /* Continue colliding until the quadtree finishes */
    rv = GFMRV_QUADTREE_OVERLAPED;
    while (rv != GFMRV_QUADTREE_DONE) {
        /** Overlaping gfmObjects */
        gfmObject *pObj1, *pObj2;
        /** gfmObjects children (if any) */
        void *pChild1, *pChild2;
        int type1, type2;
        /** Objects' types OR'd together */
        int orType;
        int isFirstCase;
        int fallthrough;

        /* Retrieve the two overlaping objects and their types */
        rv = gfmQuadtree_getOverlaping(&pObj1, &pObj2, pGlobal->pQt);
        ASSERT(rv == GFMRV_OK, rv);
        rv = collision_getSubtype(&pChild1, &type1, pObj1);
        ASSERT(rv == GFMRV_OK, rv);
        rv = collision_getSubtype(&pChild2, &type2, pObj2);
        ASSERT(rv == GFMRV_OK, rv);

#define TYPE(type) \
    (type & T_MASK)

        /* If types have at most 16 bits, one could easily OR them together to
         * use a nice case to filter collision/handle */
        orType = TYPE(type1) | (TYPE(type2) << T_BITS);
        isFirstCase = 0;

        /* Handle the collision */
        rv = GFMRV_OK;

#define CASE(type1, type2) \
    case (TYPE(type1) | (TYPE(type2) << T_BITS)): \
        if (!fallthrough) isFirstCase = 1; \
            fallthrough = 1; \
    case (TYPE(type2) | (TYPE(type1) << T_BITS)): \
            fallthrough = 1;

#define IGNORESIMPLE(type) \
    case (TYPE(type) | (TYPE(type) << T_BITS)):

#define IGNORE(type1, type2) \
    case (TYPE(type1) | (TYPE(type2) << T_BITS)): \
    case (TYPE(type2) | (TYPE(type1) << T_BITS)):

        fallthrough = 0;
        switch (orType) {
            CASE(T_TORCH_HEAD, T_LIGHT)
            CASE(T_TORCH_BOOT, T_LIGHT)
            CASE(T_TORCH_HAND, T_LIGHT) {
                gfmSprite *pTorch;
                gfmGroupNode *pNode;
                int type;

                if (isFirstCase) {
                    rv = gfmObject_getChild((void**)&pTorch, &type, pObj1);
                    ASSERT(rv == GFMRV_OK, rv);
                    pNode = (gfmGroupNode*)pChild2;
                }
                else {
                    rv = gfmObject_getChild((void**)&pTorch, &type, pObj2);
                    ASSERT(rv == GFMRV_OK, rv);
                    pNode = (gfmGroupNode*)pChild1;
                }
                torch_onCollideLight(pTorch);
                gfmGroup_removeNode(pNode);
            } break;
            CASE(T_PLAYER, T_FLOOR) {
                gfmObject *pFloor;

                if (isFirstCase) {
                    pFloor = pObj2;
                }
                else {
                    pFloor = pObj1;
                }

                player_collideFloor(pFloor);
            } break;
            CASE(T_LIGHT, T_FLOOR) {
                gfmGroupNode *pNode;

                if (isFirstCase) {
                    pNode = (gfmGroupNode*)pChild1;
                }
                else {
                    pNode = (gfmGroupNode*)pChild2;
                }
                rv = gfmGroup_removeNode(pNode);
            } break;
            CASE(T_LENS, T_LIGHT) {
                int type;
                gfmSprite *pLens, *pLight;

                if (isFirstCase) {
                    rv = gfmObject_getChild((void**)&pLens, &type, pObj1);
                    ASSERT(rv == GFMRV_OK, rv);
                    rv = gfmObject_getChild((void**)&pLight, &type, pObj2);
                    ASSERT(rv == GFMRV_OK, rv);
                }
                else {
                    rv = gfmObject_getChild((void**)&pLens, &type, pObj2);
                    ASSERT(rv == GFMRV_OK, rv);
                    rv = gfmObject_getChild((void**)&pLight, &type, pObj1);
                    ASSERT(rv == GFMRV_OK, rv);
                }
                rv = lens_reflect(pLens, pLight);
            } break;
            IGNORE(T_LIGHT, T_SOURCE)
            IGNORE(T_LENS, T_SOURCE)
            IGNORE(T_LENS, T_TORCH_HEAD)
            IGNORE(T_LENS, T_TORCH_BOOT)
            IGNORE(T_LENS, T_TORCH_HAND)
            IGNORE(T_LENS, T_TORCH_HEAD_LIT)
            IGNORE(T_LENS, T_TORCH_BOOT_LIT)
            IGNORE(T_LENS, T_TORCH_HAND_LIT)
            IGNORE(T_PLAYER, T_TORCH_HEAD)
            IGNORE(T_PLAYER, T_TORCH_BOOT)
            IGNORE(T_PLAYER, T_TORCH_HAND)
            IGNORE(T_PLAYER, T_TORCH_HEAD_LIT)
            IGNORE(T_PLAYER, T_TORCH_BOOT_LIT)
            IGNORE(T_PLAYER, T_TORCH_HAND_LIT)
            IGNORE(T_LIGHT, T_TORCH_HEAD_LIT)
            IGNORE(T_LIGHT, T_TORCH_BOOT_LIT)
            IGNORE(T_LIGHT, T_TORCH_HAND_LIT)
            IGNORE(T_FLOOR, T_TORCH_HEAD)
            IGNORE(T_FLOOR, T_TORCH_BOOT)
            IGNORE(T_FLOOR, T_TORCH_HAND)
            IGNORE(T_FLOOR, T_TORCH_HEAD_LIT)
            IGNORE(T_FLOOR, T_TORCH_BOOT_LIT)
            IGNORE(T_FLOOR, T_TORCH_HAND_LIT)
            IGNORESIMPLE(T_LIGHT)
            IGNORESIMPLE(T_SOURCE)
            IGNORESIMPLE(T_LENS)
                /* Simply ignore these cases */
            break;
            /* On Linux, a SIGINT is raised any time a unhandled collision
             * happens. When debugging, GDB will stop here and allow the user to
             * check which types weren't handled */
            default: {
#  if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
                /* Unfiltered collision, do something about it */
                raise(SIGINT);
                rv = GFMRV_INTERNAL_ERROR;
#  endif
            }
        } /* switch (orType) */
        ASSERT(rv == GFMRV_OK, rv);

        /** Update the quadtree (so any other collision is detected) */
        rv = gfmQuadtree_continue(pGlobal->pQt);
        ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE,
                rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

