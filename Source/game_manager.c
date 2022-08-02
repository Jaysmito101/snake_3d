#include "game_manager.h"

bool _game_is_initiated = false;
int _game_snake_length = 1;
int _game_snake_body[BOARD_SIZE * BOARD_SIZE];
int _game_fruit_location = -1;

bool game_init(int* board)
{
	static_assert(BOARD_SIZE >= 4, "BOARD_SIZE must be >= 4");
	memset(board, 0, sizeof(int) * BOARD_SIZE * BOARD_SIZE);
	memset(_game_snake_body, BOARD_SIZE * BOARD_SIZE, sizeof(int) * BOARD_SIZE * BOARD_SIZE);
	board[(BOARD_SIZE / 2) * BOARD_SIZE + (BOARD_SIZE / 2)] = GAME_DIR_UP;
	board[(BOARD_SIZE / 2 + 1) * BOARD_SIZE + (BOARD_SIZE / 2)] = GAME_DIR_UP;
	board[(BOARD_SIZE / 2 + 2) * BOARD_SIZE + (BOARD_SIZE / 2)] = GAME_DIR_UP;
	_game_snake_body[0] = (BOARD_SIZE / 2) * BOARD_SIZE + (BOARD_SIZE / 2);
	_game_snake_body[1] = (BOARD_SIZE / 2 + 1) * BOARD_SIZE + (BOARD_SIZE / 2);
	_game_snake_body[2] = (BOARD_SIZE / 2 + 2) * BOARD_SIZE + (BOARD_SIZE / 2);
	_game_snake_length = 3;
	srand((uint32_t)time(NULL));
	game_generate_fruit(board);
	_game_is_initiated = true;
	return _game_is_initiated;
}

int game_generate_fruit(int* board)
{
	if (_game_fruit_location > -1)
		return _game_fruit_location;

	// this is a very bad approach, a clearly
	// better approach would be to create a
	// list of the empty positions and
	// select 1 randomly from them
	while (true)
	{
		int pos = rand() % (BOARD_SIZE * BOARD_SIZE);
		if (board[pos] == 0)
		{
			board[pos] = GAME_FRUIT;
			_game_fruit_location = pos;
			printf("%d \n", pos);
			break;
		}
	}

	return _game_fruit_location;
}

bool game_update(int* old_board, int input)
{
	bool require_new_fruit = false;
	int tail = _game_snake_body[_game_snake_length - 1];
	int head = input;
	int oldHead = _game_snake_body[0]; // cache old head pos
	int newHead = oldHead;
	switch (head)
	{
	case GAME_DIR_UP:
		newHead -= BOARD_SIZE;
		break;
	case GAME_DIR_DOWN:
		newHead += BOARD_SIZE;
		break;
	case GAME_DIR_LEFT:
		newHead--;
		break;
	case GAME_DIR_RIGHT:
		newHead++;
		break;
	}
	// check if new head position is valid
	if (newHead < 0 || newHead >= BOARD_SIZE * BOARD_SIZE)
		return false;
	if (old_board[newHead] == GAME_FRUIT)
	{
		old_board[newHead] = GAME_EMPTY;
		_game_snake_body[_game_snake_length] = tail;
		_game_snake_length += 1;
		require_new_fruit = true;
		game_generate_fruit(old_board);
	}
	if (old_board[newHead] != GAME_EMPTY)
		return false;
	old_board[newHead] = input;
	if (_game_snake_length > 1)
	{
		if(!require_new_fruit)
			old_board[_game_snake_body[_game_snake_length - 1]] = GAME_EMPTY; // remove tail
		// decide new head position based on head vector
		for (int i = _game_snake_length - 1; i > 0; i--)
		{
			_game_snake_body[i] = _game_snake_body[i - 1];
		}
		_game_snake_body[0] = newHead;
	}
	else
	{
		old_board[oldHead] = GAME_EMPTY; // clear orev head position
		_game_snake_body[0] = newHead;
	}
	return true;
}

void game_shutdown()
{
	_game_is_initiated = false;
}
