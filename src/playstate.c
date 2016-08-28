
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <ld36/playstate.h>
#include <ld36/type.h>

static char *_mapKeys[] = {
    "floor"
};

static int _mapValues[] = {
    T_FLOOR
};
static const int _mapDictLen = sizeof(_mapValues) / sizeof(int);

gfmRV playstate_init() {
    gfmRV rv;

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

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV playstate_update() {
    int x;
    gfmRV rv;

    /* TODO Parallax */
    x = 0;
    rv = gfmTilemap_setPosition(pGlobal->pParallax, x, 0/*y*/);
    ASSERT(rv == GFMRV_OK, rv);

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

    rv = GFMRV_OK;
__ret:
    return rv;
}

