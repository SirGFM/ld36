/**
 * @file include/ld36/camera.h
 *
 * Camera that follows the main character but may be moved by the player.
 *
 * Whenever the camera's position is reset, it tweens to center at the player
 */
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <GFraMe/gfmCamera.h>
#include <GFraMe/gfmError.h>

struct stCamera {
    /** The internal camera */
    gfmCamera *pCamera;
    /** Camera's center */
    int x;
    /** Camera's center */
    int y;
    /** If non-negative, controls tweening from the current position to the
     * character's center */
    int time;
};
typedef struct stCamera camera_t;

/**
 * Forcefully reset the camera position to the player
 *
 * @param  [ in]width  The world's width
 * @param  [ in]height The world's height
 * @return             Return value
 */
gfmRV camera_reset(int width, int height);

/** Updates the camera position */
void camera_update();

#endif /* __CAMERA_H__ */

