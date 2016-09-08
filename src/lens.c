/**
 * @file src/lens.c
 *
 * Lens used to redirect light particles into new directions (and "replenish its
 * power"). Note that a lens is a simple gfmSprite, and it's frame/tile defines
 * its angle.
 */
#include <base/collision.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>

#include <ld36/type.h>
#include <ld36/lens.h>
#include <ld36/light.h>

gfmRV lenses_init() {
    gfmRV rv;

    rv = gfmGroup_getNew(&pGlobal->pLenses);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmGroup_setDefType(pGlobal->pLenses, T_LENS);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pGlobal->pLenses, pGfx->pSset32x32);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pGlobal->pLenses, LENSES_WIDTH, LENSES_HEIGHT
            , LENSES_OFFX, LENSES_OFFY);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pGlobal->pLenses, 0/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnTime(pGlobal->pLenses, -1/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);

    rv =  gfmGroup_setDrawOrder(pGlobal->pLenses, gfmDrawOrder_linear);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setCollisionQuality(pGlobal->pLenses
            , gfmCollisionQuality_collideEverything);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmGroup_preCache(pGlobal->pLenses, LENSES_LIST_LEN, LENSES_LIST_LEN);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

void lenses_clean() {
    gfmGroup_free(&pGlobal->pLenses);
}

gfmRV lenses_reset() {
    return gfmGroup_killAll(pGlobal->pLenses);
}

gfmRV lens_spawn(int x, int y, lensFrame frame) {
    gfmSprite *pSpr;
    gfmRV rv;

    rv = gfmGroup_recycle(&pSpr, pGlobal->pLenses);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_setPosition(pSpr, x, y);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pSpr, frame);
    ASSERT(rv == GFMRV_OK, rv);

    rv = lens_push(pSpr);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV lens_kill(gfmSprite *pLens) {
    int type;
    gfmRV rv;
    gfmGroupNode *pNode;

    rv = gfmSprite_getChild((void**)&pNode, &type, pLens);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_removeNode(pNode);
    ASSERT(rv == GFMRV_OK, rv);

    rv = lens_pop(pLens);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Update every particle */
gfmRV lenses_update() {
    gfmRV rv;

    rv = gfmGroup_update(pGlobal->pLenses, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_collideGroup(pGlobal->pQt, pGlobal->pLenses);
    ASSERT(rv == GFMRV_QUADTREE_DONE || rv == GFMRV_QUADTREE_OVERLAPED, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Render the particles */
gfmRV lenses_draw() {
    return gfmGroup_draw(pGlobal->pLenses, pGame->pCtx);
}


static int lens_checkCollision(gfmSprite *pLens, gfmSprite *pLight) {
    int lens_x, lens_y;
    int type;

    gfmSprite_getCenter(&lens_x, &lens_y, pLens);

    gfmSprite_getFrame(&type, pLens);
    switch (type) {
        case LENS_DOWN:
            return gfmSprite_overlapLine(pLight, lens_x - 12, lens_y, lens_x + 12, lens_y) == GFMRV_TRUE;
        case LENS_LEFT:
        case LENS_RIGHT:
            return gfmSprite_overlapLine(pLight, lens_x, lens_y - 12, lens_x, lens_y + 12) == GFMRV_TRUE;
        case LENS_150:
            return gfmSprite_overlapLine(pLight, lens_x - 6, lens_y - 11, lens_x + 6, lens_y + 11) == GFMRV_TRUE;
        case LENS_135:
            return gfmSprite_overlapLine(pLight, lens_x - 9, lens_y - 9, lens_x + 9, lens_y + 9) == GFMRV_TRUE;
        case LENS_45:
            return gfmSprite_overlapLine(pLight, lens_x - 9, lens_y + 9, lens_x + 9, lens_y - 9) == GFMRV_TRUE;
        case LENS_60:
            return gfmSprite_overlapLine(pLight, lens_x - 6, lens_y + 11, lens_x + 6, lens_y - 11) == GFMRV_TRUE;
        default:
            return 0;
    }
}

/**
 * Reflect a light source through a lens
 */
gfmRV lens_reflect(gfmSprite *pLens, gfmSprite *pLight) {
    int dstX, dstY, isLeft, type, x, y;
    gfmRV rv;
    gfmGroupNode *pNode;

    /* Ensure that an overlap happened */
    if (!lens_checkCollision(pLens, pLight)) {
        return GFMRV_OK;
    }

    /* Remove the last light, to spawn a new one */
    rv = gfmSprite_getChild((void**)&pNode, &type, pLight);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_removeNode(pNode);
    ASSERT(rv == GFMRV_OK, rv);

    /* Check the direction the ligth is moving into the lens */
    rv = gfmSprite_getCenter(&dstX, &dstY, pLight);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getCenter(&x, &y, pLens);
    ASSERT(rv == GFMRV_OK, rv);

    isLeft = dstX < x;

    /* Put x, y (light position) exactly above the lens */
    x -= LIGHT_RADIUS;
    y -= LIGHT_RADIUS;

    /* Calculate the correct destination */
    rv = gfmSprite_getFrame(&type, pLens);
    ASSERT(rv == GFMRV_OK, rv);

#define EXTRA_DISTANCE -1
    switch (type) {
        case LENS_DOWN: {
            y += LIGHT_RADIUS + EXTRA_DISTANCE;
            dstX = x;
            dstY = y + 1;
        } break;
        case LENS_LEFT: {
            x -= LIGHT_RADIUS + EXTRA_DISTANCE;
            dstX = x - 1;
            dstY = y;
        } break;
        case LENS_RIGHT: {
            x += LIGHT_RADIUS + EXTRA_DISTANCE;
            dstX = x + 1;
            dstY = y;
        } break;
        case LENS_45: {
            if (isLeft) {
                x += LIGHT_RADIUS + EXTRA_DISTANCE;
                y += LIGHT_RADIUS + EXTRA_DISTANCE;
                dstX = x + 1;
                dstY = y + 1;
            }
            else {
                x -= LIGHT_RADIUS + EXTRA_DISTANCE;
                y -= LIGHT_RADIUS + EXTRA_DISTANCE;
                dstX = x - 1;
                dstY = y - 1;
            }
        } break;
        case LENS_135: {
            if (isLeft) {
                x += LIGHT_RADIUS + EXTRA_DISTANCE;
                y -= LIGHT_RADIUS + EXTRA_DISTANCE;
                dstX = x + 1;
                dstY = y - 1;
            }
            else {
                x -= LIGHT_RADIUS + EXTRA_DISTANCE;
                y += LIGHT_RADIUS + EXTRA_DISTANCE;
                dstX = x - 1;
                dstY = y + 1;
            }
        } break;
        case LENS_60: {
            if (isLeft) {
                x += LIGHT_RADIUS + EXTRA_DISTANCE;
                y += LIGHT_RADIUS + EXTRA_DISTANCE;
                dstX = x + 3;
                dstY = y + 4;
            }
            else {
                x -= LIGHT_RADIUS + EXTRA_DISTANCE;
                y -= LIGHT_RADIUS + EXTRA_DISTANCE;
                dstX = x - 3;
                dstY = y - 4;
            }
        } break;
        case LENS_150: {
            if (isLeft) {
                x += LIGHT_RADIUS + EXTRA_DISTANCE;
                y -= LIGHT_RADIUS + EXTRA_DISTANCE;
                dstX = x + 3;
                dstY = y - 4;
            }
            else {
                x -= LIGHT_RADIUS + EXTRA_DISTANCE;
                y += LIGHT_RADIUS + EXTRA_DISTANCE;
                dstX = x - 3;
                dstY = y + 4;
            }
        } break;
        default: {
            return GFMRV_FUNCTION_FAILED;
        }
    }
#undef EXTRA_DISTANCE

    rv = light_spawn(x, y, dstX, dstY);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Store a lens into the array. It's used to associate a lens with a target
 */
gfmRV lens_push(gfmSprite *pSpr) {
    void *tmp;
    int i, type;
    gfmRV rv;

    i = 0;
    while (i < LENSES_LIST_LEN) {
        if (pGlobal->ppIndexedLens[i] == 0) {
            break;
        }
        i++;
    }
    ASSERT(i < LENSES_LIST_LEN, GFMRV_FUNCTION_FAILED);

    pGlobal->ppIndexedLens[i] = pSpr;
    gfmSprite_getChild(&tmp, &type, pSpr);
    type = (type & T_MASK) | (i << T_BITS);
    gfmSprite_setType(pSpr, type);

    pGlobal->lastLens = i;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Remove the stored lens.
 */
gfmRV lens_pop(gfmSprite *pLens) {
    void *tmp;
    int type, i;
    gfmRV rv;

    rv = gfmSprite_getChild(&tmp, &type, pLens);
    ASSERT(rv == GFMRV_OK, rv);

    i = type >> T_BITS;
    ASSERT(pGlobal->ppIndexedLens[i] == pLens, GFMRV_FUNCTION_FAILED);
    pGlobal->ppIndexedLens[i] = 0;

    type &= T_MASK;
    gfmSprite_setType(pLens, type);

    rv = GFMRV_OK;
__ret:
    return rv;
}

