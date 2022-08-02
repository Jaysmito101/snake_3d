#ifndef OPENGL_MANAGER_H
#define OPENGL_MANAGER_H

#include "core.h"

#include <glad/glad.h>

typedef struct
{
	uint32_t handle;
	uint32_t color;
	uint32_t depth;
} ogl_framebuffer_t;

bool ogl_init();

uint32_t ogl_create_shader(const char* source, GLenum type);
uint32_t ogl_create_shader_program(const char* vertex_shader_source, const char* fragment_shader_source);
uint32_t ogl_create_texture(uint64_t width, uint64_t height, GLenum format, GLenum internal_format, GLenum data_type);
ogl_framebuffer_t ogl_create_framebuffer(uint64_t width, uint64_t height);

void ogl_destroy_framebuffer(ogl_framebuffer_t fbo);
void ogl_shutdown();

#endif // OPENGL_MANAGER_H