#ifndef GLFW_MANAGER_H
#define GLFW_MANAGER_H

#include "core.h"


bool glfw_init();
bool glfw_setup_window(const char* title);
bool glfw_should_window_close();

void glfw_set_mouse_input_handler(void(*function)(int, int, int));
void glfw_update_window();
void glfw_shutdown();


#endif // GLFW_MANAGER_H