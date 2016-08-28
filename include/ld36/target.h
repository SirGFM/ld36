
#ifndef __TARGET_H__
#define __TARGET_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

gfmRV targets_init();

void targets_clean();

gfmRV targets_reset();

gfmRV targets_update();

gfmRV targets_draw();

gfmRV target_spawn(gfmParser *pParser);

void target_onCollision(gfmSprite *pTarget);

#endif /* __TARGET_H__ */

