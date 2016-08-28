
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gframe.h>

static const int _x[] = {64, 92,  100, 106,  22,  28, 36};
static const int _y[] = {-4, -6,  -18, -22, -22, -18, -6};

gfmRV ui_draw() {
    gfmRV rv;
    int j, i;

    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset32x32, _x[0], _y[0], 15/*tile*/
            , 0/*isFlipped*/);
    ASSERT(rv == GFMRV_OK, rv);

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

