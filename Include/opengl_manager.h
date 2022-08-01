#ifndef OPENGL_MANAGER_H
#define OPENGL_MANAGER_H

#include "core.h"

#include <glad/glad.h>

bool ogl_init();

uint32_t ogl_create_shader(const char* source, GLenum type);

uint32_t ogl_create_shader_program(const char* vertex_shader_source, const char* fragment_shader_source);

void ogl_shutdown();

#endif // OPENGL_MANAGER_H