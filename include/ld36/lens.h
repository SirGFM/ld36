/**
 * @file include/ld36/lens.h
 *
 * Lens used to redirect light particles into new directions (and "replenish its
 * power"). Note that a lens is a simple gfmSprite, and it's frame/tile defines
 * its angle.
 */
#ifndef __LENS_H__
#define __LENS_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSprite.h>

enum enLensFrame {
    LENS_HORIZONTAL = 8,
    LENS_45,
    LENS_60
};
typedef enum enLensFrame lensFrame;

gfmRV lenses_init();

void lenses_clean();

gfmRV lens_spawn(int x, int y, lensFrame frame);

gfmRV lens_kill(gfmSprite *pLens);

/**
 * Reflect a light source through a lens
 */
gfmRV lens_reflect(gfmSprite *pLens, gfmSprite *pLight);

/** Update every particle */
gfmRV lenses_update();

/** Render the particles */
gfmRV lenses_draw();

#endif /* __LENS_H__ */

