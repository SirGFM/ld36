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

/**
 * Initialize the playground state
 *
 * @return Return value
 */
gfmRV playground_init() {
    lightSourcePlayer_init(V_WIDTH / 2, 0, 10);

    return GFMRV_OK;
}

/** Clean (i.e., dealloc) the playground state */
void playground_clean() {
}

static int _time = 0;
static int _x = 0;
static int _y = V_HEIGHT / 2;

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

    if (DID_JUST_PRESS(spawn)) {
        int x, y;
        gfmInput *pInput;

        rv = gfm_getInput(&pInput, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmInput_getPointerPosition(&x, &y, pInput);
        ASSERT(rv == GFMRV_OK, rv);
        if (IS_PRESSED(left) && IS_PRESSED(power)) {
            rv = lens_spawn(x, y, LENS_135);
        }
        else if (IS_PRESSED(right) && IS_PRESSED(power)) {
            rv = lens_spawn(x, y, LENS_45);
        }
        else if (IS_PRESSED(left) && IS_PRESSED(jump)) {
            rv = lens_spawn(x, y, LENS_150);
        }
        else if (IS_PRESSED(right) && IS_PRESSED(jump)) {
            rv = lens_spawn(x, y, LENS_60);
        }
        else if (IS_PRESSED(left)) {
            rv = lens_spawn(x, y, LENS_LEFT);
        }
        else if (IS_PRESSED(right)) {
            rv = lens_spawn(x, y, LENS_RIGHT);
        }
        else /* if (IS_PRESSED(power)) */ {
            rv = lens_spawn(x, y, LENS_DOWN);
        }
        ASSERT(rv == GFMRV_OK, rv);
    }

    if ((pButton->spawn.state & gfmInput_pressed) && _time <= 0) {
        int dstX, dstY;
        gfmInput *pInput;

        rv = gfm_getInput(&pInput, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmInput_getPointerPosition(&dstX, &dstY, pInput);
        ASSERT(rv == GFMRV_OK, rv);
        rv = light_spawn(_x, _y, dstX, dstY);
        ASSERT(rv == GFMRV_OK, rv);

        _time += LIGHT_COOLDOWN;
    }
    else if (_time > 0) {
        _time -= pGame->elapsed;
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

    rv = lenses_draw();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_draw();
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

