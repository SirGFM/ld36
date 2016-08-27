
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <ld36/light.h>
#include <ld36/light_source.h>
#include <ld36/type.h>

#include <stdlib.h>
#include <string.h>

/** Alloc a single light source */
gfmRV lightSource_new(lightSource **ppPtr) {
    gfmRV rv;
    lightSource *ptr;

    ptr = malloc(sizeof(lightSource));
    ASSERT(ptr, GFMRV_ALLOC_FAILED);
    memset(ptr, 0x0, sizeof(lightSource));

    rv = gfmObject_getNew(&ptr->pArea);
    ASSERT(rv == GFMRV_OK, rv);

    *ppPtr = ptr;
    ptr = 0;
    rv = GFMRV_OK;
__ret:
    free(ptr);
    return rv;
}

/** Release memory of a single light source */
void lightSource_free(lightSource **ppPtr) {
    if (!ppPtr || !*ppPtr) {
        return;
    }

    gfmObject_free(&(*ppPtr)->pArea);
    free(*ppPtr);
    *ppPtr = 0;
}

/** Release memory used by the list of light sources */
void lightSourceList_clean() {
    gfmGenArr_clean(pGlobal->pLightSourceList, lightSource_free);
}

/** Reset the list of light sources (e.g., on game restart) */
void lightSourceList_reset() {
    gfmGenArr_reset(pGlobal->pLightSourceList);
}

/**
 * Initialze a light source on the desired position
 *
 * @param  [ in]pPtr   The light source
 * @param  [ in]x      Position of the area (upper-left corner)
 * @param  [ in]y      Position of the area (upper-left corner)
 * @param  [ in]w      Dimension of the area
 * @param  [ in]h      Dimension of the area
 * @param  [ in]spawnX Position from which light is spawned
 * @param  [ in]spawnY Position from which light is spawned
 * @param  [ in]count  How many particles should be spawned
 */
static gfmRV _lightSource_init(lightSource *pPtr, int x, int y, int w, int h
        , int spawnX, int spawnY, int count) {
    gfmRV rv;

    rv = gfmObject_clean(pPtr->pArea);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_init(pPtr->pArea, x, y, w, h, pPtr, T_SOURCE);
    ASSERT(rv == GFMRV_OK, rv);

    pPtr->spawnX = spawnX;
    pPtr->spawnY = spawnY;
    pPtr->cooldown = 0;
    pPtr->count = count;

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Alloc a new light source within the list, on the desired position
 *
 * @param  [ in]x      Position of the area (upper-left corner)
 * @param  [ in]y      Position of the area (upper-left corner)
 * @param  [ in]w      Dimension of the area
 * @param  [ in]h      Dimension of the area
 * @param  [ in]spawnX Position from which light is spawned
 * @param  [ in]spawnY Position from which light is spawned
 */
gfmRV lightSourceList_init(int x, int y, int w, int h, int spawnX, int spawnY) {
    gfmRV rv;
    lightSource *pPtr;

    gfmGenArr_getNextRef(lightSource, pGlobal->pLightSourceList, 1/*INC*/, pPtr
            , lightSource_new);

    rv = _lightSource_init(pPtr, x, y, w, h, spawnX, spawnY, 0/*count*/);
    ASSERT(rv == GFMRV_OK, rv);

    gfmGenArr_push(pGlobal->pLightSourceList);
    rv = GFMRV_OK;
__ret:
    return rv;
}

/** 
 * Initialize the player light source
 *
 * @param  [ in]x     Position from which light is spawned
 * @param  [ in]y     Position from which light is spawned
 * @param  [ in]count How many particles should be spawned
 */
void lightSourcePlayer_init(int x, int y, int count) {
    _lightSource_init(pGlobal->pPlayerSource, 0/*area-x*/, 0/*area-y*/, 8/*w*/
            , 8/*h*/, x, y, count);
}

/**
 * Reactive the light source and spawn a few particles
 *
 * @param  [ in]pPtr  The light source
 * @param  [ in]count How many particles should be spawned
 */
void lightSource_revive(lightSource *pPtr, int count) {
    if (lightSource_isAlive(pPtr)) {
        return;
    }

    pPtr->count = count;
}

/**
 * Check if the last spawned source is still alive
 *
 * @param  [ in]pPtr The light source
 * @return           1 if it's alive
 */
int lightSource_isAlive(lightSource *pPtr) {
    return pPtr && pPtr->count > 0;
}

/** Update a light source */
static gfmRV _lightSource_update(lightSource *pPtr) {
    gfmRV rv;

    if (pPtr->cooldown > 0) {
        pPtr->cooldown -= pGame->elapsed;
        return GFMRV_OK;
    }

    if (pPtr->count <= 0) {
        return GFMRV_OK;
    }
    else {
        /* Spawn a downward-moving light particle */
        rv = light_spawn(pPtr->spawnX, pPtr->spawnY, pPtr->spawnX
                , pPtr->spawnY + 1);
        ASSERT(rv == GFMRV_OK, rv);

        pPtr->cooldown += LIGHT_COOLDOWN;
        pPtr->count--;
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

/** Update every light source */
void lightSourceList_update() {
    int i;

    i = 0;
    while (i < gfmGenArr_getUsed(pGlobal->pLightSourceList)) {
        lightSource *pPtr;

        pPtr = gfmGenArr_getObject(pGlobal->pLightSourceList, i);

        _lightSource_update(pPtr);
        /* TODO Add to quadtree */

        i++;
    }

    _lightSource_update(pGlobal->pPlayerSource);
}

