/**
 * @file src/camera.c
 *
 * Camera that follows the main character but may be moved by the player.
 *
 * Whenever the camera's position is reset, it tweens to center at the player
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmCamera.h>
#include <GFraMe/gfmSprite.h>

#include <ld36/camera.h>

/**
 * Forcefully reset the camera position to the player
 *
 * @param  [ in]width  The world's width
 * @param  [ in]height The world's height
 * @return             Return value
 */
gfmRV camera_reset(int width, int height) {
    gfmRV rv;

    /* Retrieve and setup the camera */
    rv = gfm_getCamera(&pGlobal->camera.pCamera, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setWorldDimensions(pGlobal->camera.pCamera, width, height);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setDeadzone(pGlobal->camera.pCamera, DEADZONE_X, DEADZONE_Y
            , DEADZONE_W, DEADZONE_H);
    ASSERT(rv == GFMRV_OK, rv);

    /* Center the camera at the player */
    gfmSprite_getCenter(&pGlobal->camera.x, &pGlobal->camera.y
            , pGlobal->player.pSelf);
    gfmCamera_centerAtPoint(pGlobal->camera.pCamera, pGlobal->camera.x
            , pGlobal->camera.y);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Updates the camera position */
void camera_update() {
    gfmSprite_getCenter(&pGlobal->camera.x, &pGlobal->camera.y
            , pGlobal->player.pSelf);
    gfmCamera_centerAtPoint(pGlobal->camera.pCamera, pGlobal->camera.x
            , pGlobal->camera.y);
}

