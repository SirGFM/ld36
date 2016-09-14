/**
 * @file src/playground.c
 *
 * Test state, for checking new features
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <ld36/lens.h>
#include <ld36/light.h>
#include <ld36/light_source.h>
#include <ld36/playground.h>

struct stTest {
    int srcX, srcY;
    int dstX, dstY;
    gfmSprite *pLens;
    lensFrame frame;
    int time;
} test;

/**
 * Initialize the playground state
 *
 * @return Return value
 */
gfmRV playground_init() {
    gfmRV rv;

    /* Retrieve a single lens, that'll be controlable */
    test.frame = LENS_DOWN;
    rv = lenses_reset();
    ASSERT(rv == GFMRV_OK, rv);
    rv = lens_spawn(V_WIDTH / 2, V_HEIGHT / 2, test.frame);
    ASSERT(rv == GFMRV_OK, rv);
    test.pLens = pGlobal->ppIndexedLens[pGlobal->lastLens];

    test.srcX = V_WIDTH / 2;
    test.srcY = 0;
    test.dstX = V_WIDTH / 2;
    test.dstY = V_HEIGHT / 2;
    test.time = 0;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Clean (i.e., dealloc) the playground state */
void playground_clean() {
}

/**
 * Update the playground state
 *
 * @return Return value
 */
gfmRV playground_update() {
    gfmRV rv;

    rv = gfmQuadtree_initRoot(pGlobal->pQt, -8/*x*/, -8/*y*/, V_WIDTH+16
            , V_HEIGHT+16, QT_MAX_DEPTH, QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);

    /* Update the lens frame */
    if (DID_JUST_PRESS(cycleLeft)) {
        test.frame--;
        if (test.frame <= LENS_MIN) {
            test.frame = LENS_MAX - 1;
        }
    }
    else if (DID_JUST_PRESS(cycleRight)) {
        test.frame++;
        if (test.frame >= LENS_MAX) {
            test.frame = LENS_MIN + 1;
        }
    }
    rv = gfmSprite_setFrame(test.pLens, test.frame);
    ASSERT(rv == GFMRV_OK, rv);

    /* Move the light source */
    if (IS_PRESSED(left)) {
        test.srcX--;
    }
    else if (IS_PRESSED(right)) {
        test.srcX++;
    }
    if (IS_PRESSED(up)) {
        test.srcY--;
    }
    else if (IS_PRESSED(down)) {
        test.srcY++;
    }

    if (IS_PRESSED(cameraLeft)) {
        test.dstX--;
    }
    else if (IS_PRESSED(cameraRight)) {
        test.dstX++;
    }
    if (IS_PRESSED(cameraUp)) {
        test.dstY--;
    }
    else if (IS_PRESSED(cameraDown)) {
        test.dstY++;
    }

    /* Spawn light */
    if (IS_PRESSED(spawn) && test.time <= 0) {
        rv = light_spawn(test.srcX, test.srcY, test.dstX, test.dstY);
        ASSERT(rv == GFMRV_OK, rv);

        test.time += LIGHT_COOLDOWN;
    }
    else if (test.time > 0) {
        test.time -= pGame->elapsed;
    }

    rv = lenses_update();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_update();
    ASSERT(rv == GFMRV_OK, rv);
    lightSourceList_update();

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Render the playground state
 *
 * @return Return value
 */
gfmRV playground_draw() {
    gfmRV rv;

    /* Draw src & dst targets */
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, test.srcX, test.srcY
            , 'O' - '!', 0);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, test.dstX, test.dstY
            , 'X' - '!', 0);
    ASSERT(rv == GFMRV_OK, rv);

    rv = lenses_draw();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_draw();
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

