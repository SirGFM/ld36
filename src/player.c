
#include <base/collision.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <ld36/lens.h>
#include <ld36/player.h>
#include <ld36/type.h>

static int _player_animData[] = {
       /* len | fps | loop | frames */
/*stand */ 2  ,  4  ,  1   , 64,65,
/* jump */ 1  ,  0  ,  0   , 66,
/* walk */ 2  ,  6  ,  1   , 66,67,
/*lookup*/ 1  ,  0  ,  0   , 80,
/* hold */ 1  ,  0  ,  0   , 81,
/* fall */ 1  ,  0  ,  0   , 82,
};
static const int _player_animData_len = sizeof(_player_animData) / sizeof(int);

gfmRV player_init(gfmParser *pParser) {
    int x, y;
    gfmRV rv;

    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);

    y -= PLAYER_HEIGHT + 1;
    rv = gfmSprite_init(pGlobal->pPlayer, x, y, PLAYER_WIDTH, PLAYER_HEIGHT
            , pGfx->pSset16x16, PLAYER_OFFX, PLAYER_OFFY, 0, T_PLAYER);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_addAnimations(pGlobal->pPlayer, _player_animData
            , _player_animData_len);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pGlobal->pPlayer, PL_STAND);
    ASSERT(rv == GFMRV_OK, rv);

    gfmSprite_setVerticalAcceleration(pGlobal->pPlayer, DOWN_GRAV);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Physical update & collision handling */
static gfmRV _player_internalUpdate() {
    int x;
    gfmRV rv;

    rv = gfmSprite_update(pGlobal->pPlayer, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    /* Limit to world space */
    gfmSprite_getHorizontalPosition(&x, pGlobal->pPlayer);
    if (x <= 0) {
        gfmSprite_setHorizontalPosition(pGlobal->pPlayer, 0);
    }
    else if (x + PLAYER_WIDTH >= pGlobal->worldWidth) {
        gfmSprite_setHorizontalPosition(pGlobal->pPlayer
                , pGlobal->worldWidth - PLAYER_WIDTH);
    }

    rv = gfmQuadtree_collideSprite(pGlobal->pQt, pGlobal->pPlayer);
    ASSERT(rv == GFMRV_QUADTREE_DONE || rv == GFMRV_QUADTREE_OVERLAPED, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}


gfmRV player_preUpdate() {
    double vx, vy;
    gfmRV rv;
    gfmCollision dir, lastDir;

    /* NOTE: TARGET MUST HAVE PRECEDENCE OVER THE PLAYER!! */
    gfmSprite_getCollision(&dir, pGlobal->pPlayer);
    if ((dir & gfmCollision_down) && DID_JUST_PRESS(action)
            && !pGlobal->didAct) {
        pGlobal->didAct = 1;
        /* If the player was carrying a lens, drop it */
        if (pGlobal->playerLensIndex != -1) {
            gfmSprite *pLens;

            pLens = pGlobal->ppIndexedLens[pGlobal->playerLensIndex];
            rv = lens_kill(pLens);
            ASSERT(rv == GFMRV_OK, rv);
            pGlobal->playerLensIndex = -1;
            pGlobal->playerCurLens++;
        }
        else if (pGlobal->playerCurLens > 0) {
            int isFlipped;
            int x, y;

            gfmSprite_getDirection(&isFlipped, pGlobal->pPlayer);
            gfmSprite_getCenter(&x, &y, pGlobal->pPlayer);
            if (isFlipped) {
                x -= PLAYER_LENS_DISTX;
            }
            else {
                x += PLAYER_LENS_DISTX;
            }
            x -= LENSES_WIDTH / 2;
            y += PLAYER_LENS_DISTY - LENSES_HEIGHT / 2;

            rv = lens_spawn(x, y, LENS_RIGHT);
            ASSERT(rv == GFMRV_OK, rv);

            pGlobal->playerLensIndex = pGlobal->lastLens;
            pGlobal->playerCurLens--;
        }
    }

    /** Skip the update, since the player can't move if holding a mirror */
    if (pGlobal->playerLensIndex >= 0) {
        gfmSprite_setHorizontalVelocity(pGlobal->pPlayer, 0.0);
        return _player_internalUpdate();
    }

    if (IS_PRESSED(left)) {
        vx = -PLAYER_VX;
        gfmSprite_setDirection(pGlobal->pPlayer, 1/*isFlipped*/);
    }
    else if (IS_PRESSED(right)) {
        vx = PLAYER_VX;
        gfmSprite_setDirection(pGlobal->pPlayer, 0/*isFlipped*/);
    }
    else {
        vx = 0.0;
    }
    gfmSprite_setHorizontalVelocity(pGlobal->pPlayer, vx);

    gfmSprite_getCollision(&dir, pGlobal->pPlayer);
    gfmSprite_getLastCollision(&lastDir, pGlobal->pPlayer);
    if (DID_JUST_PRESS(jump) && ((dir & gfmCollision_down)
            || (lastDir & gfmCollision_down))) {
        gfmSprite_setVerticalVelocity(pGlobal->pPlayer, PLAYER_JUMP);
        gfmSprite_setVerticalAcceleration(pGlobal->pPlayer, UP_GRAV);
    }

    gfmSprite_getVerticalVelocity(&vy, pGlobal->pPlayer);
    if (vy > 0) {
        gfmSprite_setVerticalAcceleration(pGlobal->pPlayer, DOWN_GRAV);
    }

    /* Physical update & collision handling */
    rv = _player_internalUpdate();
__ret:
    return rv;
}

void player_postUpdate() {
    double vx, vy;
    int x, y;

    gfmSprite_getVelocity(&vx, &vy, pGlobal->pPlayer);

    if (0) {
    }
    else if (pGlobal->playerLensIndex != -1) {
        gfmSprite_playAnimation(pGlobal->pPlayer, PL_HOLD);
    }
    else if (vx == 0.0 && vy == 0.0) {
        gfmSprite_playAnimation(pGlobal->pPlayer, PL_STAND);
    }
    else if (vy < 0.0) {
        gfmSprite_playAnimation(pGlobal->pPlayer, PL_JUMP);
    }
    else if (vy > 0.0) {
        gfmSprite_playAnimation(pGlobal->pPlayer, PL_FALL);
    }
    else if (vx != 0.0) {
        gfmSprite_playAnimation(pGlobal->pPlayer, PL_WALK);
    }

    gfmSprite_getCenter(&x, &y, pGlobal->pPlayer);
    gfmCamera_centerAtPoint(pGlobal->pCamera, x, y);
}


gfmRV player_draw() {
    return gfmSprite_draw(pGlobal->pPlayer, pGame->pCtx);
}


void player_collideFloor(gfmObject *pFloor) {
    gfmObject *pObj = 0;
    double vy;
    gfmCollision dir;

    gfmSprite_getObject(&pObj, pGlobal->pPlayer);
    if (gfmObject_isOverlaping(pObj, pFloor) == GFMRV_FALSE) {
        return;
    }

    gfmObject_getVerticalVelocity(&vy, pObj);

    gfmObject_collide(pObj, pFloor);
    gfmObject_getCurrentCollision(&dir, pObj);
    if (dir & gfmCollision_down) {
        gfmObject_setVerticalVelocity(pObj, 0.0);
    }
    else if (dir & gfmCollision_up) {
        int y;

        gfmObject_setVerticalVelocity(pObj, 0.0);
        gfmSprite_getVerticalPosition(&y, pGlobal->pPlayer);
        gfmSprite_setVerticalPosition(pGlobal->pPlayer, y+1);
    }
}

