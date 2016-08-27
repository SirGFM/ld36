/**
 * @file include/ld36/light.h
 *
 * Handles light particles
 */
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <GFraMe/gfmError.h>

/**
 * Initialize the light particles
 *
 * @return GFraMe return value
 */
gfmRV light_init();

/**
 * Reset the particles
 *
 * @return GFraMe return value
 */
gfmRV light_reset();

/**
 * Spawn a light source moving toward a given direction
 *
 * @param  [ in]srcX Particle's center
 * @param  [ in]srcY Particle's center
 * @param  [ in]dstX Destination
 * @param  [ in]dstY Destination
 */
gfmRV light_spawn(int srcX, int srcY, int dstX, int dstY);

/** Update every particle */
gfmRV light_update();

/** Render the particles */
gfmRV light_draw();

/** Clean the light particles */
void light_clean();

#endif /* __LIGHT_H__ */

