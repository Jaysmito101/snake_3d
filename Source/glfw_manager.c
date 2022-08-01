#include "glfw_manager.h"

#include <GLFW/glfw3.h>

bool _glfw_is_initiated = false;
GLFWwindow* _glfw_main_window = NULL;
void(*_glfw_mouse_input_handler)(int, int, int) = NULL;

// This function will be used by glfw to report any internal errors
static void __glfw_error_callback(int error, const char* description)
{
	printf("ERROR: (glfw) %d : %s", error, description);
}

// the glfw mouse position callback function
static void __glfw_mouse_move_callback(GLFWwindow* window, double mouse_x, double mouse_y)
{
	if (_glfw_mouse_input_handler)
	{
		_glfw_mouse_input_handler(
			-1, // the mouse button -1 means not button is being pressed
			(int)mouse_x,
			(int)mouse_y
			);
	}
}

// the glfw mouse button callback function
static void __glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mod)
{
	if (_glfw_mouse_input_handler)
	{
		double mouse_x = 0.0, mouse_y = 0.0;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		_glfw_mouse_input_handler(
			button,
			(int)mouse_x,
			(int)mouse_y
		);
	}
}


// initialize and setup glfw 
bool glfw_init()
{
	if (_glfw_is_initiated)
		return true;

	if (!glfwInit())
	{
		printf("ERROR: Failed to initialize GLFW\n");
		_glfw_is_initiated = false;
		return _glfw_is_initiated;
	}


	glfwSetErrorCallback(__glfw_error_callback);

	_glfw_is_initiated = true;
	return _glfw_is_initiated;
}

// create the main window
bool glfw_setup_window(const char* title)
{
	if (!_glfw_is_initiated)
		return false;

	// tell glfw we are going to use opengl api
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	// tell glfw to initialize opengl context for opengl version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// tell glfw to use the opengl core profile and not the compatibility profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// disabling resing gets rid of managing things like aspect ration and stuff
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_glfw_main_window = glfwCreateWindow(512, 512, title, NULL, NULL);
	glfwMakeContextCurrent(_glfw_main_window);

	return true;
}

// tell whether the close button on the window is clicked
bool glfw_should_window_close()
{
	return glfwWindowShouldClose(_glfw_main_window);
}

void glfw_set_mouse_input_handler(void(*function)(int, int, int))
{
	// if thw window has not yet been setup there is no point in setting up input handlers
	if (!_glfw_main_window)
		return;

	// if no input handler has been set that means this is the first time this
	// function is being called thus first it is needed to call the underlaying
	// glfw functions to setup the underlyaing callbacks
	if (!_glfw_mouse_input_handler)
	{
		glfwSetCursorPosCallback(_glfw_main_window, __glfw_mouse_move_callback);
		glfwSetMouseButtonCallback(_glfw_main_window, __glfw_mouse_button_callback);
	}

	_glfw_mouse_input_handler = function;
}

// poll the events and swap the buffers (if not called continously the window will show up as not responding)
void glfw_update_window()
{
	glfwSwapBuffers(_glfw_main_window);
	glfwPollEvents();
}

// clean up glfw
void glfw_shutdown()
{
	if (_glfw_main_window)
		glfwDestroyWindow(_glfw_main_window);

	if (_glfw_is_initiated)
		glfwTerminate();
}
