
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
#include <ld36/torch.h>
#include <ld36/type.h>

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

    rv = lenses_reset();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_reset();
    ASSERT(rv == GFMRV_OK, rv);
    rv = torches_reset();
    ASSERT(rv == GFMRV_OK, rv);

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
        }
        else if (IS_TYPE("target")) {
        }
        else {
            rv = GFMRV_FUNCTION_NOT_IMPLEMENTED;
        }
        ASSERT(rv == GFMRV_OK, rv);
#undef IS_TYPE
    }

    rv = GFMRV_OK;
__ret:
    gfmParser_free(&pParser);

    return rv;
}


gfmRV playstate_update() {
    int x, w, h;
    gfmRV rv;

    rv = gfmTilemap_getDimension(&w, &h, pGlobal->pMap);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_initRoot(pGlobal->pQt, 0/*x*/, 0/*y*/, w, h
            , QT_MAX_DEPTH, QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_populateTilemap(pGlobal->pQt, pGlobal->pMap);
    ASSERT(rv == GFMRV_OK, rv);

    rv = torches_update();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_update();
    ASSERT(rv == GFMRV_OK, rv);

    rv = player_preUpdate();
    ASSERT(rv == GFMRV_OK, rv);

    /* Parallax */
    rv = gfmSprite_getHorizontalPosition(&x, pGlobal->pPlayer);
    ASSERT(rv == GFMRV_OK, rv);
    x = -(x / (V_WIDTH / 8));
    if (x > 0) x = 0;
    rv = gfmTilemap_setPosition(pGlobal->pParallax, x, 0/*y*/);
    ASSERT(rv == GFMRV_OK, rv);

    player_postUpdate();

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

    rv = torches_draw();
    ASSERT(rv == GFMRV_OK, rv);
    rv = player_draw();
    ASSERT(rv == GFMRV_OK, rv);
    rv = light_draw();
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

