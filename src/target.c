
#include <base/collision.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <ld36/target.h>
#include <ld36/type.h>


gfmRV targets_init() {
    gfmRV rv;

    rv = gfmGroup_getNew(&pGlobal->pTargets);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmGroup_setDefType(pGlobal->pTargets, 0x7FFF0000 | T_TARGET);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pGlobal->pTargets, 0/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnTime(pGlobal->pTargets, -1/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv =  gfmGroup_setDrawOrder(pGlobal->pTargets, gfmDrawOrder_linear);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setCollisionQuality(pGlobal->pTargets
            , gfmCollisionQuality_collideEverything);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pGlobal->pTargets, pGfx->pSset8x32);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pGlobal->pTargets, 14/*w*/, 25/*h*/, 4/*offx*/
            , -4/*offy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pGlobal->pTargets, MAX_TARGETS, MAX_TARGETS);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

void targets_clean() {
    gfmGroup_free(&pGlobal->pTargets);
}

gfmRV targets_reset() {
    return gfmGroup_killAll(pGlobal->pTargets);
}

gfmRV targets_update() {
    gfmRV rv;

    rv = gfmGroup_update(pGlobal->pTargets, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_collideGroup(pGlobal->pQt, pGlobal->pTargets);
    ASSERT(rv == GFMRV_QUADTREE_DONE || rv == GFMRV_QUADTREE_OVERLAPED, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV targets_draw() {
    return gfmGroup_draw(pGlobal->pTargets, pGame->pCtx);
}


gfmRV target_spawn(gfmParser *pParser) {
    gfmSprite *pSpr = 0;
    int x, y;
    gfmRV rv;

    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);

    /* Retrieve the sprite to be used */
    rv = gfmGroup_recycle(&pSpr, pGlobal->pTargets);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setFrame(pGlobal->pTargets, 127/*frame*/);
    ASSERT(rv == GFMRV_OK, rv);

    y += V_HEIGHT / 2;

    y -= 28;
    rv = gfmSprite_setPosition(pSpr, x, y);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

void target_onCollision(gfmSprite *pTarget) {
    int type, index;
    gfmRV rv;
    gfmGroupNode *pNode;

    if (!DID_JUST_PRESS(action)) {
        return;
    }

    rv = gfmSprite_getChild((void**)&pNode, &type, pTarget);
    ASSERT(rv == GFMRV_OK, rv);

    index = type >> T_BITS;
    type &= T_MASK;

    if (index == 0x7FFF && pGlobal->playerCurLens > 0) {
        int x, y;
        gfmSprite_getCenter(&x, &y, pTarget);

        x -= 7;
        y -= 19;
        rv = lens_spawn(x, y, pGlobal->curLensDir);
        ASSERT(rv == GFMRV_OK, rv);

        index = pGlobal->lastLens;
        pGlobal->playerCurLens--;
    }
    else if (index != 0x7FFF) {
        gfmSprite *pLens;

        pLens = pGlobal->ppIndexedLens[index];
        rv = lens_kill(pLens);
        ASSERT(rv == GFMRV_OK, rv);
        index = 0x7FFF;
        pGlobal->playerCurLens++;
    }

    type |= index << T_BITS;
    gfmSprite_setType(pTarget, type);

    pGlobal->didAct = 1;
__ret:
    return;
}

