#ifndef RENDERER_MANAGER_H
#define RENDERER_MANAGER_H

#include "core.h"

bool renderer_init();

void renderer_update_game_board(int* old_board, int* new_board);

void renderer_render(float animation_percentage);

void renderer_shutdown();

#endif // RENDERER_MANAGER_H