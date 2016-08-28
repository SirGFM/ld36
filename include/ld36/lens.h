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
    LENS_LEFT = 8,
    LENS_150,
    LENS_135,
    LENS_DOWN,
    LENS_45,
    LENS_60,
    LENS_RIGHT,
    LENS_MAX
};
typedef enum enLensFrame lensFrame;

gfmRV lenses_init();

void lenses_clean();

gfmRV lenses_reset();

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

