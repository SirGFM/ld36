/**
 * @file src/light.c
 *
 * Handles light particles
 */
#include <base/collision.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmGroup.h>

#include <ld36/light.h>
#include <ld36/type.h>

#include <math.h>

static int _light_data[] = {
/* len | fps | loop | frames */
    13 ,  6  ,   0  , 16,17,18,19,20,21,22,23,24,25,26,27,28
};
static const int _light_data_len = sizeof(_light_data) / sizeof(int);

/**
 * Initialize the light particles
 *
 * @return GFraMe return value
 */
gfmRV light_init() {
    gfmRV rv;

    /* TODO Allow shrinking the number (for performance reasons) */
    pGlobal->lightLen = MAX_LIGHT_LEN;

    rv = gfmGroup_getNew(&pGlobal->pLight);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmGroup_setDefType(pGlobal->pLight, T_LIGHT);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pGlobal->pLight, pGfx->pSset16x16);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefAnimData(pGlobal->pLight, _light_data, _light_data_len);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pGlobal->pLight, 8/*w*/, 8/*h*/, -4/*offx*/
            , -4/*offy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pGlobal->pLight, 0/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnTime(pGlobal->pLight, LIGHT_TTL);
    ASSERT(rv == GFMRV_OK, rv);

    /* No need for fancy draw orders, since every particle is equal */
    rv =  gfmGroup_setDrawOrder(pGlobal->pLight, gfmDrawOrder_linear);
    ASSERT(rv == GFMRV_OK, rv);

    /* TODO Set this according to the current quality */
    rv = gfmGroup_setCollisionQuality(pGlobal->pLight
            , gfmCollisionQuality_collideEverything);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmGroup_preCache(pGlobal->pLight, pGlobal->lightLen
            , pGlobal->lightLen);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Reset the particles
 *
 * @return GFraMe return value
 */
gfmRV light_reset() {
    return gfmGroup_killAll(pGlobal->pLight);
}

/**
 * Spawn a light source moving toward a given direction
 *
 * @param  [ in]srcX Particle's center
 * @param  [ in]srcY Particle's center
 * @param  [ in]dstX Destination
 * @param  [ in]dstY Destination
 */
gfmRV light_spawn(int srcX, int srcY, int dstX, int dstY) {
    gfmRV rv;
    double vx, vy, norm;
    gfmSprite *pSpr;

    rv = gfmGroup_recycle(&pSpr, pGlobal->pLight);
    ASSERT(rv == GFMRV_OK, rv);

    /* Normalize the speed */
    vx = (double)(dstX - srcX);
    vy = (double)(dstY - srcY);
    norm = 1 / sqrt(vx * vx + vy * vy);
    vx = LIGHT_VELOCITY * vx * norm;
    vy = LIGHT_VELOCITY * vy * norm;

    rv = gfmSprite_setPosition(pSpr, srcX, srcY);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setVelocity(pSpr, vx, vy);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pSpr, 0);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_resetAnimation(pSpr);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Update every particle */
gfmRV light_update() {
    gfmRV rv;

    rv = gfmGroup_update(pGlobal->pLight, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_collideGroup(pGlobal->pQt, pGlobal->pLight);
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
gfmRV light_draw() {
    return gfmGroup_draw(pGlobal->pLight, pGame->pCtx);
}

/** Clean the light particles */
void light_clean() {
    gfmGroup_free(&pGlobal->pLight);
}

