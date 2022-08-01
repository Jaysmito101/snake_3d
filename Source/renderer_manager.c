#define _CRT_SECURE_NO_WARNINGS

#include "renderer_manager.h"
#include "opengl_manager.h"

// vertices for a screen quad (2 triangles covering the whole screen)
static float vertices[] = {
	 1.0f,  1.0f, 0.0f,  // top right
	 1.0f, -1.0f, 0.0f,  // bottom right
	-1.0f, -1.0f, 0.0f,  // bottom left
	-1.0f,  1.0f, 0.0f   // top left 		
};

static int32_t indices[] = {
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};

bool _renderer_is_initiated = false;

uint32_t _vao = 0; // vertex array
uint32_t _vbo = 0; // vertex buffer
uint32_t _ebo = 0; // index buffer
uint32_t _shd = 0; // shader program

struct {
	uint32_t resolution;
} uniforms;

/*
#version 430 core
layout(location = 0) in vec3 Position;
void main()
{
	gl_Position = vec4(Position, 1.0f);
}
*/
const char* vertex_shader_source = "#version 460 core\n\tlayout (location = 0) in vec3 Position;  \n\tvoid main()\n\t{\n\t\tgl_Position = vec4(Position, 1.0f);\n\t}\n";

const char* fragment_shader_source = "";

// temporary function just for development
static char* read_file(const char* file_path)
{
	FILE* f = fopen(file_path, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* string = malloc(fsize + 1);
	fread(string, fsize, 1, f);
	fclose(f);
	string[fsize] = 0;
	return string;
}

bool renderer_init()
{
	if (_renderer_is_initiated)
		return true;

	// generate and bind the vertex array
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// generate the vertex buffer
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// generate the index buffer
	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// we only have a single 3d vector (3 floats) for each vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// create the shader
	
	// _shd = ogl_create_shader_program(vertex_shader_source, fragment_shader_source);
	
	char* fragment_shader_source_T = read_file("shader.glsl");
	_shd = ogl_create_shader_program(vertex_shader_source, fragment_shader_source_T);
	free(fragment_shader_source_T);

	if (!_shd)
		return false;

	// load up the uniforms in advance
	uniforms.resolution = glGetUniformLocation(_shd, "_Resolution");

	_renderer_is_initiated = true;
	return _renderer_is_initiated;
}

void renderer_render()
{
	// bind the shader
	glUseProgram(_shd);
	
	// update the uniforms
	glUniform2f(uniforms.resolution, 512.0f, 512.0f);

	// bind the veertex array and render the quad
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void renderer_shutdown()
{
	if (!_renderer_is_initiated)
		return;

	// free the vertex array
	glDeleteVertexArrays(1, &_vao);
	// free the vertex buffer
	glDeleteBuffers(1, &_vbo);
	// free the index buffer
	glDeleteBuffers(1, &_ebo);
	// free the shader
	glDeleteShader(_shd);
	
	_renderer_is_initiated = false;
}