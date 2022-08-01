#include "core.h"
#include "glfw_manager.h"
#include "opengl_manager.h"
#include "renderer_manager.h"
#include "game_manager.h"

void on_mouse(int mouse_button, int mouse_x, int mouse_y)
{

}

int main(int argc, char** argv, char** envp)
{
	if (!glfw_init())
		exit(EXIT_FAILURE);
	if(!glfw_setup_window("Snake [Jaysmito Mukherjee]"))
		exit(EXIT_FAILURE);

	ogl_init();

	glfw_set_mouse_input_handler(on_mouse);

	renderer_init();
	
	while (!glfw_should_window_close())
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind the default framebuffer (not necessarily needed)
		glViewport(0, 0, 512, 512); // out main viwport (the window) has fixed size of 512 x 512 pixels

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // clear the screen with a light shade of grey
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear both the color and depth attachments

		renderer_render();

		glfw_update_window();
	}

	renderer_shutdown();

	ogl_shutdown();

	glfw_shutdown();

	return EXIT_SUCCESS;

}