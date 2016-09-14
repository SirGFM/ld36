
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gframe.h>

#define CENTER_X 0
#define CENTER_Y 0

/* Positions for lenses, roughly in a circle */
#define FIX_FRAME(n) (n - LENS_LEFT)
static const int _frame[] = {
      [FIX_FRAME(LENS_LEFT)]  = 42
    , [FIX_FRAME(LENS_135)]   = 43
    , [FIX_FRAME(LENS_DOWN)]  = 44
    , [FIX_FRAME(LENS_45)]    = 45
    , [FIX_FRAME(LENS_RIGHT)] = 58
};

static gfmRV _draw_lenses() {
    gfmRV rv;

    /* Draw lens' frame */
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset32x32, -3, -3, 15/*tile*/
            , 0/*isFlipped*/);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset16x16, 5, 5
            , _frame[FIX_FRAME(pGlobal->curLensDir)] , 0/*isFlipped*/);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV _draw_count() {
    gfmRV rv;

#define BASE_X 261
#define BASE_Y -5

    /* Draw number of lenses being carried */
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset64x32, BASE_X/*x*/, BASE_Y/*y*/
            , 10/*tile*/, 0/*isFlipped*/);
    ASSERT(rv == GFMRV_OK, rv);

#define DRAW_CHAR(x, y, c) \
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, BASE_X + x, BASE_Y + y, \
            (c) - '!', 0); \
    ASSERT(rv == GFMRV_OK, rv)
    DRAW_CHAR(8, 8, 'L');
    DRAW_CHAR(16, 8, 'E');
    DRAW_CHAR(24, 8, 'N');
    DRAW_CHAR(32, 8, 'S');
    DRAW_CHAR(40, 8, ':');
    DRAW_CHAR(48, 8, pGlobal->player.curLens + '0');

    DRAW_CHAR(16, 16, 'M');
    DRAW_CHAR(24, 16, 'A');
    DRAW_CHAR(32, 16, 'X');
    DRAW_CHAR(40, 16, ':');
    DRAW_CHAR(48, 16, pGlobal->player.maxLens + '0');

#undef BASE_X
#undef BASE_Y
#define BASE_X 124
#define BASE_Y (240 / 2)

    if (pGlobal->torchCount <= 0) {
        DRAW_CHAR( 0, 0, 'G');
        DRAW_CHAR( 8, 0, 'A');
        DRAW_CHAR(16, 0, 'M');
        DRAW_CHAR(24, 0, 'E');
        DRAW_CHAR(40, 0, 'O');
        DRAW_CHAR(48, 0, 'V');
        DRAW_CHAR(56, 0, 'E');
        DRAW_CHAR(64, 0, 'R');
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}


gfmRV ui_draw() {
    gfmRV rv;

    if (pGlobal->player.maxLens > 0) {
        rv = _draw_lenses();
        ASSERT(rv == GFMRV_OK, rv);
    }
    rv = _draw_count();
__ret:
    return rv;
}

