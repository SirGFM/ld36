/**
 * @file include/base/game_const.h
 *
 * Defines all constants
 */
#ifndef __GAME_CONST_H__
#define __GAME_CONST_H__

/* TODO Set attributes for the game */

/* == Game's basic configurations =========================================== */

/** Game's "organization" */
#define ORG         "com.gfmgamecorner"
/** Game's title */
#define TITLE       "Burning Light"
/** Config file name */
#define CONF        "config.sav"
/** Virtual window's width */
#define V_WIDTH     160
/** Virtual window's height */
#define V_HEIGHT    120
/** Whether the user can resize the game window */
#define CAN_RESIZE  1
/** Initial background color in 0xAARRGGBB format */
#define BG_COLOR    0xFF5B6EE1
/** Atlas texture */
#define TEXATLAS    "gfx/atlas.bmp"
/** Spriteset used by the FPS counter. It's ignored, if NULL */
#define FPS_SSET    pGfx->pSset8x8
/** First ASCII tile in FPS_SSET */
#define FPS_INIT    0
/** FPS counter position. It must be enabled through a 'gfm_showFPSCounter',
 * which is called by default on debug mode */
#define FPS_X       0
#define FPS_Y       0

/* == Config file IDs ======================================================= */

#define CONF_ID_INIT        "init"
#define CONF_ID_FLAGS       "flags"
#define CONF_ID_RES         "res"
#define CONF_ID_WIDTH       "width"
#define CONF_ID_HEIGHT      "height"
#define CONF_ID_FPS         "fps"
#define CONF_ID_AUDIOQ      "audio"
#define CONF_ID_LAST_FLAGS  "lFlags"
#define CONF_ID_LAST_RES    "lRes"
#define CONF_ID_LAST_WIDTH  "lWidth"
#define CONF_ID_LAST_HEIGHT "lHeight"
#define CONF_ID_LAST_FPS    "lFps"
#define CONF_ID_LAST_AUDIOQ "lAudio"

/* == Default configuration ================================================= */

#define CONF_FLAGS  (CFG_VSYNC)
#define CONF_RES    0
#define CONF_WIDTH  640
#define CONF_HEIGHT 480
#define CONF_FPS    60
#define CONF_AUDIOQ gfmAudio_defQuality

/* == ... =================================================================== */

/** Texture's transparent color */
#define KEY_COLOR       0xFF00FF
/** Maximum number of times the quadtree will subdivide */
#define QT_MAX_DEPTH    6
/** Number of nodes required for the quadtree to subdivide */
#define QT_MAX_NODES    10

/* == ... =================================================================== */

/** At most 4k sprites for light particles, and it takes 0.5s to traverse the
 * screen. If it were to last 4 screens, it would have to live for 2s.
 * If being spawned in a stream, every particle will be distant by 1.5 times its
 * radius*/
#define MAX_LIGHT_LEN   4096
#define LIGHT_RADIUS    4
#define LIGHT_TTL       3000
#define LIGHT_SCREEN_TRAVERSE_TIME  1.125
#define LIGHT_DISTANCE_IN_RADIUS    2
#define LIGHT_OFFX      -4
#define LIGHT_OFFY      -4

#define LIGHT_VELOCITY  (V_WIDTH / LIGHT_SCREEN_TRAVERSE_TIME)
#define LIGHT_COOLDOWN  (1000 * LIGHT_RADIUS * LIGHT_DISTANCE_IN_RADIUS \
                        / LIGHT_VELOCITY)

#define LENSES_LIST_LEN 32
#define LENS_RADIUS     10
#define LENSES_WIDTH    16
#define LENSES_HEIGHT   16
#define LENSES_OFFX     -8
#define LENSES_OFFY     -8

#define TILE_SIZE       8
#define MAP_WIDTH       200
#define MAP_HEIGHT      15
#define MAP_FILE        "maps/map_platforms.gfm"
#define PARALLAX_FILE   "maps/map_parallax.gfm"
#define OBJECTS_FILE    "maps/map_objects.gfm"

#define PLAYER_WIDTH    4
#define PLAYER_HEIGHT   10
#define PLAYER_OFFX     -6
#define PLAYER_OFFY     -5
#define PLAYER_LENS_DISTX   3
#define PLAYER_LENS_DISTY   -8

#define PLAYER_TILES_PER_SEC    8

#define UP_GRAV          380
#define DOWN_GRAV        525

#define PLAYER_VX       (((double)TILE_SIZE) * PLAYER_TILES_PER_SEC)
#define PLAYER_JUMP     -165

#define MAX_TORCHES     10
#define TORCH_WIDTH     8
#define TORCH_HEIGHT    8
/* Number of light particles required to lit the torch */
#define TORCH_POWER     10

#endif /* __GAME_CONST_H__ */

