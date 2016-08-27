/**
 * @file include/ld36/light_source.h
 *
 * Area that spawns light from a point if the player is within it
 */
#ifndef __ST_LIGHT_SOURCE_H__
#define __ST_LIGHT_SOURCE_H__

#include <GFraMe/gfmGenericArray.h>
typedef struct stLightSource lightSource;
gfmGenArr_define(lightSource);

#endif /* __ST_LIGHT_SOURCE_H__ */


#ifndef __LIGHT_SOURCE_H__
#define __LIGHT_SOURCE_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmObject.h>

struct stLightSource {
    gfmObject *pArea;
    int spawnX;
    int spawnY;
    int cooldown;
    int count;
};

/** Alloc a single light source */
gfmRV lightSource_new(lightSource **ppPtr);

/** Release memory of a single light source */
void lightSource_free(lightSource **ppPtr);

/** Release memory used by the list of light sources */
void lightSourceList_clean();

/** Reset the list of light sources (e.g., on game restart) */
void lightSourceList_reset();

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
gfmRV lightSourceList_init(int x, int y, int w, int h, int spawnX, int spawnY);

/** 
 * Initialize the player light source
 *
 * @param  [ in]x     Position from which light is spawned
 * @param  [ in]y     Position from which light is spawned
 * @param  [ in]count How many particles should be spawned
 */
void lightSourcePlayer_init(int x, int y, int count);

/**
 * Reactive the light source and spawn a few particles
 *
 * @param  [ in]pPtr  The light source
 * @param  [ in]count How many particles should be spawned
 */
void lightSource_revive(lightSource *pPtr, int count);

/**
 * Check if the last spawned source is still alive
 *
 * @param  [ in]pPtr The light source
 * @return           1 if it's alive
 */
int lightSource_isAlive(lightSource *pPtr);

/** Update a light source */
gfmRV lightSource_update(lightSource *pPtr);

/** Update every light source on the list */
void lightSourceList_update();

#endif /* __LIGHT_SOURCE_H__ */

