
#include <base/collision.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <ld36/lens.h>
#include <ld36/light.h>
#include <ld36/player.h>
#include <ld36/playstate.h>
#include <ld36/target.h>
#include <ld36/torch.h>
#include <ld36/type.h>
#include <ld36/ui.h>

#include <string.h>

static char *_mapKeys[] = {
    "floor"
};

static int _mapValues[] = {
    T_FLOOR
};
static const int _mapDictLen = sizeof(_mapValues) / sizeof(int);


gfmRV playstate_init() {
    gfmParser *pParser = 0;
    gfmRV rv;

    /* Reset stuff */
    rv = lenses_reset();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_reset();
    ASSERT(rv == GFMRV_OK, rv);
    rv = torches_reset();
    ASSERT(rv == GFMRV_OK, rv);
    rv = targets_reset();
    ASSERT(rv == GFMRV_OK, rv);
    pGlobal->torchCount = 0;
    pGlobal->playerLensIndex = -1;
    pGlobal->playerMaxLens = INITIAL_TARGETS;
    pGlobal->playerCurLens = INITIAL_TARGETS;
    memset(pGlobal->ppIndexedLens, 0x0, sizeof(gfmSprite*) * LENSES_LIST_LEN);

    pGlobal->curLensDir = LENS_DOWN;

    /* Load Map & parallax */
    rv = gfmTilemap_init(pGlobal->pMap, pGfx->pSset8x8, MAP_WIDTH, MAP_HEIGHT
            , -1/*defTilemap*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_loadf(pGlobal->pMap, pGame->pCtx, MAP_FILE
            , sizeof(MAP_FILE)-1, _mapKeys, _mapValues, _mapDictLen);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmTilemap_init(pGlobal->pParallax, pGfx->pSset8x8, MAP_WIDTH
            , MAP_HEIGHT, -1/*defTilemap*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_loadf(pGlobal->pParallax, pGame->pCtx, PARALLAX_FILE
            , sizeof(PARALLAX_FILE)-1, _mapKeys, _mapValues, _mapDictLen);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmTilemap_getDimension(&pGlobal->worldWidth, &pGlobal->worldHeight
            , pGlobal->pMap);
    ASSERT(rv == GFMRV_OK, rv);
    /* TODO Correctly get this value */
    pGlobal->worldWidth = 108 * 8;
    pGlobal->worldHeight = V_HEIGHT;


    /* Parse stuff */
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_initStatic(pParser, pGame->pCtx, OBJECTS_FILE);
    ASSERT(rv == GFMRV_OK, rv);
    while (1) {
        char *key;
        gfmParserType type;

        rv = gfmParser_parseNext(pParser);
        ASSERT(rv == GFMRV_OK || rv == GFMRV_PARSER_FINISHED, rv);
        if (rv == GFMRV_PARSER_FINISHED) {
            break;
        }

        rv = gfmParser_getType(&type, pParser);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmParser_getIngameType(&key, pParser);
        ASSERT(rv == GFMRV_OK, rv);

#define IS_TYPE(name) (strcmp(key, name) == 0)
        if (IS_TYPE("player")) {
            rv = player_init(pParser);
        }
        else if (IS_TYPE("torch")) {
            rv = torch_spawn(pParser);
            pGlobal->torchCount++;
        }
        else if (IS_TYPE("target")) {
            rv = target_spawn(pParser);
        }
        else if (IS_TYPE("source")) {
            int x, y;

            rv = gfmParser_getPos(&x, &y, pParser);
            ASSERT(rv == GFMRV_OK, rv);
            rv = lightSourceList_init(x-32, 0, 64
                    , pGlobal->worldHeight, x, y);
        }
        else {
            rv = GFMRV_FUNCTION_NOT_IMPLEMENTED;
        }
        ASSERT(rv == GFMRV_OK, rv);
#undef IS_TYPE
    }

    rv = gfm_getCamera(&pGlobal->pCamera, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setWorldDimensions(pGlobal->pCamera, pGlobal->worldWidth
            , pGlobal->worldHeight);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setDeadzone(pGlobal->pCamera, DEADZONE_X, DEADZONE_Y
            , DEADZONE_W, DEADZONE_H);
    ASSERT(rv == GFMRV_OK, rv);


    rv = GFMRV_OK;
__ret:
    gfmParser_free(&pParser);

    return rv;
}


gfmRV playstate_update() {
    int x;
    gfmRV rv;

    pGlobal->didAct = 0;
    pGlobal->lastLens = -1;

    /* Check if should cycle the lens direction */
    if (DID_JUST_PRESS(cycleLeft)) {
        pGlobal->curLensDir--;
    }
    if (DID_JUST_PRESS(cycleRight)) {
        pGlobal->curLensDir++;
    }
    /* Cap the current lens direction */
    if (pGlobal->curLensDir <= LENS_MIN) {
        pGlobal->curLensDir = LENS_MAX - 1;
    }
    else if (pGlobal->curLensDir >= LENS_MAX) {
        pGlobal->curLensDir = LENS_MIN + 1;
    }

    rv = gfmQuadtree_initRoot(pGlobal->pQt, 0/*x*/, 0/*y*/, pGlobal->worldWidth
            , pGlobal->worldHeight, QT_MAX_DEPTH, QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_populateTilemap(pGlobal->pQt, pGlobal->pMap);
    ASSERT(rv == GFMRV_OK, rv);

#if defined(DEBUG)
    if (DID_JUST_PRESS(spawn)) {
        int dstX, dstY;
        gfmInput *pInput;

        rv = gfm_getInput(&pInput, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmInput_getPointerPosition(&dstX, &dstY, pInput);
        ASSERT(rv == GFMRV_OK, rv);
        rv = light_spawn(V_WIDTH/2-4, 0, dstX, dstY);
        ASSERT(rv == GFMRV_OK, rv);
    }
#endif

    lightSourceList_update();

    rv = torches_update();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_update();
    ASSERT(rv == GFMRV_OK, rv);
    rv = lenses_update();
    ASSERT(rv == GFMRV_OK, rv);
    rv = targets_update();
    ASSERT(rv == GFMRV_OK, rv);

    rv = player_preUpdate();
    ASSERT(rv == GFMRV_OK, rv);

    /* Parallax */
    rv = gfmSprite_getHorizontalPosition(&x, pGlobal->pPlayer);
    ASSERT(rv == GFMRV_OK, rv);
    x = -(x / (V_WIDTH / 4));
    if (x > 0) x = 0;
    rv = gfmTilemap_setPosition(pGlobal->pParallax, x, 0);
    ASSERT(rv == GFMRV_OK, rv);

    player_postUpdate();

    if (pGlobal->torchCount <= 0) {
        pGlobal->endX += 180.0 * pGame->elapsed / 1000.0;
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}


gfmRV playstate_draw() {
    gfmRV rv;

    rv = gfmTilemap_draw(pGlobal->pParallax, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_draw(pGlobal->pMap, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = ui_draw();
    ASSERT(rv == GFMRV_OK, rv);

    rv = targets_draw();
    ASSERT(rv == GFMRV_OK, rv);

    if (pGlobal->torchCount > 0) {
        rv = torches_draw();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = player_draw();
    ASSERT(rv == GFMRV_OK, rv);
    rv = lenses_draw();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_draw();
    ASSERT(rv == GFMRV_OK, rv);

    if (pGlobal->torchCount <= 0) {
#define DRAW_END(x, y, sset, tile) \
    rv = gfm_drawTile(pGame->pCtx, pGfx->sset, x + (int)pGlobal->endX, y \
            , tile, 1/*flip*/); \
    ASSERT(rv == GFMRV_OK, rv)

        /* HEAD */
        DRAW_END(-32, 30, pSset32x64, 11);
        /* BACK HAND */
        DRAW_END(-72, 40, pSset16x32, 60);
        /* FRONT HAND */
        DRAW_END(-120, 28, pSset16x32, 60);
        /* FRONT BOOT */
        DRAW_END(-160, 60, pSset32x32, 29);
        /* BACK BOOT */
        DRAW_END(-210, 70, pSset32x32, 28);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

