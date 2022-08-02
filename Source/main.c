#include "core.h"
#include "glfw_manager.h"
#include "opengl_manager.h"
#include "renderer_manager.h"
#include "game_manager.h"

#include <GLFW/glfw3.h>

int _input = GAME_DIR_UP;

void on_mouse(int mouse_button, int mouse_x, int mouse_y)
{

}

void on_key(int button)
{
	switch (button)
	{
	case GLFW_KEY_UP:
	case GLFW_KEY_W:
		_input = GAME_DIR_UP;
		break;
	case GLFW_KEY_DOWN:
	case GLFW_KEY_S:
		_input = GAME_DIR_DOWN;
		break;
	case GLFW_KEY_LEFT:
	case GLFW_KEY_A:
		_input = GAME_DIR_LEFT;
		break;
	case GLFW_KEY_RIGHT:
	case GLFW_KEY_D:
		_input = GAME_DIR_RIGHT;
		break;
	default:
		break;
	}
}

int main(int argc, char** argv, char** envp)
{
	if (!glfw_init())
		exit(EXIT_FAILURE);
	if(!glfw_setup_window("Snake [Jaysmito Mukherjee]"))
		exit(EXIT_FAILURE);

	ogl_init();

	glfw_set_mouse_input_handler(on_mouse);
	glfw_set_key_input_handler(on_key);

	renderer_init();
	
	int board[BOARD_SIZE * BOARD_SIZE * 2];

	game_init(board);

	double previousTime = glfwGetTime();
	double timerDefault = 1.0f;
	double timer = timerDefault;
	double animPercent = 0.0f;
	bool playing = true;

	while (!glfw_should_window_close())
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - previousTime;
		previousTime = currentTime;
		if (playing)
		{
			timer -= deltaTime;
			animPercent = (timerDefault - timer) / timerDefault;
			if (timer < 0.0f)
			{
				playing = game_update(board, _input);
				renderer_update_game_board(board, board + (BOARD_SIZE * BOARD_SIZE));
				timer = timerDefault;
			}
		}

		if (animPercent >= 1.00f) animPercent = 0.0f;

		renderer_render((float)animPercent);

		glfw_update_window();
	}

	game_shutdown();

	renderer_shutdown();

	ogl_shutdown();

	glfw_shutdown();

	return EXIT_SUCCESS;

}