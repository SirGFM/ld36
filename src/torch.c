
#include <base/collision.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <ld36/torch.h>
#include <ld36/type.h>

#include <string.h>

#define ANIM(name, ...) \
    /* len | fps | loop | data */ \
        1  ,  0  ,  0   , __VA_ARGS__
static int _torch_data[] = {
    ANIM(HEAD, 24),
    ANIM(HEAD_LIT, 25),
    ANIM(HAND, 52),
    ANIM(HAND_LIT, 53),
    ANIM(BOOT, 36),
    ANIM(BOOT_LIT, 37),
};
static const int _torch_data_len = sizeof(_torch_data) / sizeof(int);

gfmRV torches_init() {
    gfmRV rv;

    rv = gfmGroup_getNew(&pGlobal->pTorches);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmGroup_setDefType(pGlobal->pTorches, T_TORCH);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefAnimData(pGlobal->pTorches, _torch_data
            , _torch_data_len);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pGlobal->pTorches, 0/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnTime(pGlobal->pTorches, -1/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv =  gfmGroup_setDrawOrder(pGlobal->pTorches, gfmDrawOrder_linear);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setCollisionQuality(pGlobal->pTorches
            , gfmCollisionQuality_collideEverything);
    ASSERT(rv == GFMRV_OK, rv);
    /* Anything goes for the spriteset and the dimensions, since they are
     * overwritten when a new torch is spawned */
    rv = gfmGroup_setDefSpriteset(pGlobal->pTorches, pGfx->pSset16x16);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pGlobal->pTorches, 2/*w*/, 2/*h*/, 0/*offx*/
            , 0/*offy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pGlobal->pTorches, MAX_TORCHES, MAX_TORCHES);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

void torches_clean() {
    gfmGroup_free(&pGlobal->pTorches);
}

gfmRV torches_reset() {
    return gfmGroup_killAll(pGlobal->pTorches);
}

gfmRV torches_update() {
    gfmRV rv;

    rv = gfmGroup_update(pGlobal->pTorches, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_collideGroup(pGlobal->pQt, pGlobal->pTorches);
    ASSERT(rv == GFMRV_QUADTREE_DONE || rv == GFMRV_QUADTREE_OVERLAPED, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV torches_draw() {
    return gfmGroup_draw(pGlobal->pTorches, pGame->pCtx);
}


gfmRV torch_spawn(gfmParser *pParser) {
    gfmSprite *pSpr = 0;
    gfmSpriteset *pSset;
    char *key, *val;
    int anim, num, offX, offY, torch, x, y;
    gfmRV rv;

    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_getNumProperties(&num, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(num == 1, rv);

    /* Retrieve the specific torch type */
    rv = gfmParser_getProperty(&key, &val, pParser, 0/*index*/);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(strcmp(key, "part") == 0, GFMRV_FUNCTION_FAILED);
    if (strcmp(val, "head") == 0) {
        torch = T_TORCH_HEAD;
        anim = HEAD;
        pSset = pGfx->pSset32x32;
        offX = -12;
        offY = -6;
        x += 12;
        y -= 18 + TORCH_HEIGHT;
    }
    else if (strcmp(val, "hand") == 0) {
        torch = T_TORCH_HAND;
        anim = HAND;
        pSset = pGfx->pSset16x32;
        offX = -4;
        offY = -10;
        x += 4;
        y -= 14 + TORCH_HEIGHT;
    }
    else if (strcmp(val, "boot") == 0) {
        torch = T_TORCH_BOOT;
        anim = BOOT;
        pSset = pGfx->pSset16x32;
        offX = -4;
        offY = -10;
        x += 4;
        y -= 14 + TORCH_HEIGHT;
    }

    /* Retrieve the sprite to be used */
    rv = gfmGroup_recycle(&pSpr, pGlobal->pTorches);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_setPosition(pSpr, x, y);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setDimensions(pSpr, TORCH_WIDTH, TORCH_HEIGHT);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setOffset(pSpr, offX, offY);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setSpriteset(pSpr, pSset);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setType(pSpr, torch);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pSpr, anim);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

void torch_onCollideLight(gfmSprite *pSelf) {
    void *tmp = 0;
    int type, power;

    gfmSprite_getChild(&tmp, &type, pSelf);

    /* Extract only the power (hit count) from the type and increase it */
    power = type >> T_BITS;
    type &= T_MASK;
    power++;

    if (power >= TORCH_POWER) {
        int anim;

        switch (type) {
            case T_TORCH_HEAD: {
                type = T_TORCH_HEAD_LIT;
                anim = HEAD_LIT;
            } break;
            case T_TORCH_BOOT: {
                type = T_TORCH_BOOT_LIT;
                anim = BOOT_LIT;
            } break;
            case T_TORCH_HAND: {
                type = T_TORCH_HAND_LIT;
                anim = HAND_LIT;
            } break;
        }

        pGlobal->torchCount--;
        gfmSprite_playAnimation(pSelf, anim);

        if (pGlobal->torchCount <= 0) {
            /*ASJDGASGKJ*/
            pGlobal->endX  = 0;
        }
    }
    else {
        type |= power << T_BITS;
    }

    gfmSprite_setType(pSelf, type);
}

