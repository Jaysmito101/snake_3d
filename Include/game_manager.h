#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "core.h"

#define BOARD_SIZE			  16

#define GAME_FRUIT		     -1
#define GAME_EMPTY			  0
#define GAME_DIR_UP			  1
#define GAME_DIR_DOWN		  2
#define GAME_DIR_LEFT		  3
#define GAME_DIR_RIGHT		  4


bool game_init(int* board);
bool game_update(int* old_board, int input);
int  game_generate_fruit(int* board);
void game_shutdown();

#endif // GAME_MANAGER_H