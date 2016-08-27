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

    rv = gfmGroup_preCache(pGlobal->pLenses, LENSES_LEN,LENSES_LEN);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

void lenses_clean() {
    gfmGroup_free(&pGlobal->pLenses);
}

gfmRV lens_spawn(int x, int y, lensFrame frame) {
    gfmRV rv;
    gfmSprite *pSpr;

    rv = gfmGroup_recycle(&pSpr, pGlobal->pLenses);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_setPosition(pSpr, x, y);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pSpr, frame);
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

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Reflect a light source through a lens
 */
gfmRV lens_reflect(gfmSprite *pLens, gfmSprite *pLight) {
    int centerX, centerY, dstX, dstY, lastX, lastY, type, x, y;
    gfmRV rv;
    gfmGroupNode *pNode;

    /* TODO Ensure that an overlap happened */

    /* Calculate the moving direction */
    rv = gfmSprite_getCenter(&centerX, &centerY, pLight);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getLastCenter(&lastX, &lastY, pLight);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_getPosition(&x, &y, pLight);
    ASSERT(rv == GFMRV_OK, rv);

    /* Remove the last light, to spawn a new one */
    rv = gfmSprite_getChild((void**)&pNode, &type, pLight);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_removeNode(pNode);
    ASSERT(rv == GFMRV_OK, rv);

    /* TODO Calculate the correct destination */
    dstX = x + centerY - lastY;
    dstY = y + centerX - lastX;

    rv = light_spawn(x, y, dstX, dstY);
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

