
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

enum enPlayerAnim {
    PL_STAND = 0,
    PL_JUMP,
    PL_WALK,
    PL_LOOKUP,
    PL_HOLD,
    PL_FALL,
    PL_MAX
};
typedef enum enPlayerAnim playerAnim;

gfmRV player_init(gfmParser *pParser);
gfmRV player_preUpdate();
void player_postUpdate();
gfmRV player_draw();
void player_collideFloor(gfmObject *pFloor);

#endif /* __PLAYER_H__*/

