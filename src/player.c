
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
    rv = gfmSprite_init(pGlobal->player.pSelf, x, y, PLAYER_WIDTH, PLAYER_HEIGHT
            , pGfx->pSset16x16, PLAYER_OFFX, PLAYER_OFFY, 0, T_PLAYER);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_addAnimations(pGlobal->player.pSelf, _player_animData
            , _player_animData_len);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pGlobal->player.pSelf, PL_STAND);
    ASSERT(rv == GFMRV_OK, rv);

    gfmSprite_setVerticalAcceleration(pGlobal->player.pSelf, DOWN_GRAV);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Physical update & collision handling */
static gfmRV _player_internalUpdate() {
    int x;
    gfmRV rv;

    rv = gfmSprite_update(pGlobal->player.pSelf, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    /* Limit to world space */
    gfmSprite_getHorizontalPosition(&x, pGlobal->player.pSelf);
    if (x <= 0) {
        gfmSprite_setHorizontalPosition(pGlobal->player.pSelf, 0);
    }
    else if (x + PLAYER_WIDTH >= pGlobal->worldWidth) {
        gfmSprite_setHorizontalPosition(pGlobal->player.pSelf
                , pGlobal->worldWidth - PLAYER_WIDTH);
    }

    rv = gfmQuadtree_collideSprite(pGlobal->pQt, pGlobal->player.pSelf);
    ASSERT(rv == GFMRV_QUADTREE_DONE || rv == GFMRV_QUADTREE_OVERLAPED, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV _playerAction() {
    gfmRV rv;
    gfmCollision dir;

    gfmSprite_getCollision(&dir, pGlobal->player.pSelf);
    if ((dir & gfmCollision_down) && DID_JUST_PRESS(action)
            && !pGlobal->didAct) {
        pGlobal->didAct = 1;
        /* If the player was carrying a lens, drop it */
        if (pGlobal->player.playerLensIndex != -1) {
            gfmSprite *pLens;

            pLens = pGlobal->ppIndexedLens[pGlobal->player.playerLensIndex];
            rv = lens_kill(pLens);
            ASSERT(rv == GFMRV_OK, rv);
            pGlobal->player.playerLensIndex = -1;
            pGlobal->player.playerCurLens++;
        }
        else if (pGlobal->player.playerCurLens > 0) {
            int isFlipped;
            int x, y;

            gfmSprite_getDirection(&isFlipped, pGlobal->player.pSelf);
            gfmSprite_getCenter(&x, &y, pGlobal->player.pSelf);
            if (isFlipped) {
                x -= PLAYER_LENS_DISTX;
            }
            else {
                x += PLAYER_LENS_DISTX;
            }
            x -= LENSES_WIDTH / 2;
            y += PLAYER_LENS_DISTY - LENSES_HEIGHT / 2;

            rv = lens_spawn(x, y, pGlobal->curLensDir);
            ASSERT(rv == GFMRV_OK, rv);

            pGlobal->player.playerLensIndex = pGlobal->lastLens;
            pGlobal->player.playerCurLens--;
        }
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}


gfmRV player_preUpdate() {
    double vx, vy;
    gfmRV rv;
    gfmCollision dir, lastDir;

    /** Skip the update, since the player can't move if holding a mirror */
    if (pGlobal->player.playerLensIndex >= 0) {
        gfmSprite *pLens;

        pLens = pGlobal->ppIndexedLens[pGlobal->player.playerLensIndex];
        /* Fix the lens' frame/dir */
        rv = gfmSprite_setFrame(pLens, pGlobal->curLensDir);
        ASSERT(rv == GFMRV_OK, rv);

        gfmSprite_setHorizontalVelocity(pGlobal->player.pSelf, 0.0);
        return _player_internalUpdate();
    }
    if ( pGlobal->torchCount <= 0) {
        gfmSprite_setHorizontalVelocity(pGlobal->player.pSelf, 0.0);
        return _player_internalUpdate();
    }

    if (IS_PRESSED(left)) {
        vx = -PLAYER_VX;
        gfmSprite_setDirection(pGlobal->player.pSelf, 1/*isFlipped*/);
    }
    else if (IS_PRESSED(right)) {
        vx = PLAYER_VX;
        gfmSprite_setDirection(pGlobal->player.pSelf, 0/*isFlipped*/);
    }
    else {
        vx = 0.0;
    }
    gfmSprite_setHorizontalVelocity(pGlobal->player.pSelf, vx);

    gfmSprite_getCollision(&dir, pGlobal->player.pSelf);
    gfmSprite_getLastCollision(&lastDir, pGlobal->player.pSelf);
    if (DID_JUST_PRESS(jump) && ((dir & gfmCollision_down)
            || (lastDir & gfmCollision_down))) {
        gfmSprite_setVerticalVelocity(pGlobal->player.pSelf, PLAYER_JUMP);
        gfmSprite_setVerticalAcceleration(pGlobal->player.pSelf, UP_GRAV);
    }

    gfmSprite_getVerticalVelocity(&vy, pGlobal->player.pSelf);
    if (vy > 0) {
        gfmSprite_setVerticalAcceleration(pGlobal->player.pSelf, DOWN_GRAV);
    }

    /* Physical update & collision handling */
    rv = _player_internalUpdate();
__ret:
    return rv;
}

void player_postUpdate() {
    double vx, vy;
    int x, y;

    /* NOTE: TARGET MUST HAVE PRECEDENCE OVER THE PLAYER!! */
    _playerAction();

    gfmSprite_getVelocity(&vx, &vy, pGlobal->player.pSelf);

    if (0) {
    }
    else if (pGlobal->player.playerLensIndex != -1) {
        gfmSprite_playAnimation(pGlobal->player.pSelf, PL_HOLD);
    }
    else if (vx == 0.0 && vy == 0.0) {
        gfmSprite_playAnimation(pGlobal->player.pSelf, PL_STAND);
    }
    else if (vy < 0.0) {
        gfmSprite_playAnimation(pGlobal->player.pSelf, PL_JUMP);
    }
    else if (vy > 0.0) {
        gfmSprite_playAnimation(pGlobal->player.pSelf, PL_FALL);
    }
    else if (vx != 0.0) {
        gfmSprite_playAnimation(pGlobal->player.pSelf, PL_WALK);
    }

    gfmSprite_getCenter(&x, &y, pGlobal->player.pSelf);
    gfmCamera_centerAtPoint(pGlobal->pCamera, x, y);
}


gfmRV player_draw() {
    return gfmSprite_draw(pGlobal->player.pSelf, pGame->pCtx);
}


void player_collideFloor(gfmObject *pFloor) {
    gfmObject *pObj = 0;
    double vy;
    int x;
    gfmCollision dir;

    gfmSprite_getObject(&pObj, pGlobal->player.pSelf);
    if (gfmObject_isOverlaping(pObj, pFloor) == GFMRV_FALSE) {
        return;
    }

    gfmObject_getVerticalVelocity(&vy, pObj);

    gfmSprite_getHorizontalPosition(&x, pGlobal->player.pSelf);
    gfmObject_collide(pObj, pFloor);
    gfmObject_getCurrentCollision(&dir, pObj);
    if (dir & gfmCollision_down) {
        gfmObject_setVerticalVelocity(pObj, 0.0);
    }
    else if (dir & gfmCollision_up) {
        int y;

        gfmObject_setVerticalVelocity(pObj, 0.0);
        gfmSprite_getVerticalPosition(&y, pGlobal->player.pSelf);
        gfmSprite_setVerticalPosition(pGlobal->player.pSelf, y+1);
    }

    if (dir & gfmCollision_left) {
        gfmSprite_setHorizontalPosition(pGlobal->player.pSelf, x + 1);
    }
    else if (dir & gfmCollision_right) {
        gfmSprite_setHorizontalPosition(pGlobal->player.pSelf, x - 0);
    }
}

