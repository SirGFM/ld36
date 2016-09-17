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
    int x, y;
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
    pGlobal->camera.x = 0;
    pGlobal->camera.y = 0;
    pGlobal->camera.w = width;
    pGlobal->camera.h = height;
    gfmSprite_getCenter(&x, &y, pGlobal->player.pSelf);
    gfmCamera_centerAtPoint(pGlobal->camera.pCamera, x, y);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Updates the camera position */
void camera_update() {
    int x, y;
    int repeat = 1;

    gfmSprite_getCenter(&x, &y, pGlobal->player.pSelf);

    do {
        gfmRV rv;

        /* TODO Allow higher FPSs than 60 */
        if (DID_JUST_PRESS(cameraReset)) {
            pGlobal->camera.x = 0;
            pGlobal->camera.y = 0;
            repeat = 0;
        }
        else if (IS_PRESSED(cameraLeft)) {
            pGlobal->camera.x -= pGame->elapsed / 8;
            if (x + pGlobal->camera.x < 0) {
                pGlobal->camera.x = -x;
            }
        }
        else if (IS_PRESSED(cameraRight)) {
            pGlobal->camera.x += pGame->elapsed / 8;
            if (x + pGlobal->camera.x > pGlobal->camera.w) {
                pGlobal->camera.x = pGlobal->camera.w - x;
            }
        }
        if (IS_PRESSED(cameraUp)) {
            pGlobal->camera.y -= pGame->elapsed / 8;
            if (y + pGlobal->camera.y < 0) {
                pGlobal->camera.y = -y;
            }
        }
        else if (IS_PRESSED(cameraDown)) {
            pGlobal->camera.y += pGame->elapsed / 8;
            if (y + pGlobal->camera.y > pGlobal->camera.h) {
                pGlobal->camera.y = pGlobal->camera.h - y;
            }
        }

        if (!(IS_PRESSED(cameraLeft) || IS_PRESSED(cameraRight)
                || IS_PRESSED(cameraUp) || IS_PRESSED(cameraDown))) {
            /* The camera is centering on the player, so just run once */
            repeat = 0;
        }

        rv = gfmCamera_centerAtPoint(pGlobal->camera.pCamera
                , x + pGlobal->camera.x, y + pGlobal->camera.y);
        if (rv == GFMRV_CAMERA_MOVED
                || (IS_PRESSED(cameraLeft) && x + pGlobal->camera.x <= 0)
                || (IS_PRESSED(cameraRight)
                    && x + pGlobal->camera.x >= pGlobal->camera.w)
                || (IS_PRESSED(cameraUp) && y + pGlobal->camera.y <= 0)
                || (IS_PRESSED(cameraDown)
                    && y + pGlobal->camera.y >= pGlobal->camera.h)) {
            /* When changing directions, it's required to reposition the camera
             * offset at the deadzone edge */
            repeat = 0;
        }
    } while (repeat == 1);
}

