
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmSprite.h>

#include <ld36/light_source.h>

struct stPlayer {
    /** Light source that the player may spawn anywhere */
    lightSource *pPlayerSource;
    /** Player's sprite */
    gfmSprite *pSelf;
    /** Player's current animation (avoids repeating) */
    int playerAnim;
    /** Index, on pGlobal->ppIndexedLens, of the lens which the player is
     * holding (if any) */
    int playerLensIndex;
    /** How many lenses the player still has (i.e., max - number on targets) */
    int playerCurLens;
    /** How many lenses the player has */
    int playerMaxLens;
};
typedef struct stPlayer player_t;

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

