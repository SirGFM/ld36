/**
 * @file include/ld36/playground.h
 *
 * Test state, for checking new features
 */
#ifndef __PLAYGROUND_H__
#define __PLAYGROUND_H__

#include <GFraMe/gfmError.h>

/**
 * Initialize the playground state
 *
 * @return Return value
 */
gfmRV playground_init();

/** Clean (i.e., dealloc) the playground state */
void playground_clean();

/**
 * Update the playground state
 *
 * @return Return value
 */
gfmRV playground_update();

/**
 * Render the playground state
 *
 * @return Return value
 */
gfmRV playground_draw();

#endif /* __PLAYGROUND_H__ */

