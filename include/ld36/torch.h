
#ifndef __TORCH_H__
#define __TORCH_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

enum enTorchAnims {
    HEAD = 0,
    HEAD_LIT,
    HAND,
    HAND_LIT,
    BOOT,
    BOOT_LIT,
};
typedef enum enTorchAnims torchAnims;

gfmRV torches_init();

void torches_clean();

gfmRV torches_reset();

gfmRV torches_update();

gfmRV torches_draw();

gfmRV torch_spawn(gfmParser *pParser);

#endif /* __TORCH_H__ */

