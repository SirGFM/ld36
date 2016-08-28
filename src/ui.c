
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gframe.h>

/* Positions for lenses, roughly in a circle */
static const int _x[] = {64, 92,  100, 106,  22,  28, 36};
static const int _y[] = {-4, -6,  -18, -22, -22, -18, -6};

static gfmRV _draw_lenses() {
    gfmRV rv;
    int j, i;

    /* Draw lens' frame */
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset32x32, _x[0], _y[0], 15/*tile*/
            , 0/*isFlipped*/);
    ASSERT(rv == GFMRV_OK, rv);

    /* Draw lenses in order */
    j = 0;
    i = pGlobal->curLensDir;
    while (i < LENS_MAX) {
        rv = gfm_drawTile(pGame->pCtx, pGfx->pSset32x32, _x[j], _y[j], i/*tile*/
                , 0/*isFlipped*/);
        ASSERT(rv == GFMRV_OK, rv);
        j++;
        i++;
    }
    i = LENS_MIN + 1;
    while (i < pGlobal->curLensDir) {
        rv = gfm_drawTile(pGame->pCtx, pGfx->pSset32x32, _x[j], _y[j], i/*tile*/
                , 0/*isFlipped*/);
        ASSERT(rv == GFMRV_OK, rv);
        j++;
        i++;
    }


    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV _draw_count() {
    gfmRV rv;

    /* Draw number of lenses being carried */
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset64x32, 101/*x*/, -5/*y*/
            , 10/*tile*/, 0/*isFlipped*/);
    ASSERT(rv == GFMRV_OK, rv);

#define DRAW_CHAR(x, y, c) \
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, x, y, (c) - '!', 0); \
    ASSERT(rv == GFMRV_OK, rv)
    DRAW_CHAR(109, 3, 'L');
    DRAW_CHAR(117, 3, 'E');
    DRAW_CHAR(125, 3, 'N');
    DRAW_CHAR(133, 3, 'S');
    DRAW_CHAR(141, 3, ':');
    DRAW_CHAR(149, 3, pGlobal->playerCurLens + '0');

    DRAW_CHAR(117, 11, 'M');
    DRAW_CHAR(125, 11, 'A');
    DRAW_CHAR(133, 11, 'X');
    DRAW_CHAR(141, 11, ':');
    DRAW_CHAR(149, 11, pGlobal->playerMaxLens + '0');

    rv = GFMRV_OK;
__ret:
    return rv;
}


gfmRV ui_draw() {
    gfmRV rv;

    if (pGlobal->playerMaxLens > 0) {
        rv = _draw_lenses();
        ASSERT(rv == GFMRV_OK, rv);
    }
    rv = _draw_count();
__ret:
    return rv;
}

