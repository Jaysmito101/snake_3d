#define _CRT_SECURE_NO_WARNINGS

#include "renderer_manager.h"
#include "opengl_manager.h"
#include "game_manager.h"
#include "shaders.h"

#define GAME_FRAME_RESOLUTION 512

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

uint32_t			_vao		= 0; // vertex array
uint32_t			_vbo		= 0; // vertex buffer
uint32_t			_ebo		= 0; // index buffer
uint32_t			_ssb		= 0; // the buffer to store th board
uint32_t			_shd		= 0; // shader program to render the main game
uint32_t			_shd_copy	= 0; // shader program to copy main render from
									 // alternate framebuffer to window framebuffer
ogl_framebuffer_t	_fbo;

struct {
	// from main shader
	uint32_t resolution;
	uint32_t boardSize;
	uint32_t animationPercentage;

	// from copy shader
	uint32_t frame_copy;
	uint32_t resolution_copy;
} uniforms;


/*
// temporary function just for development
static char* read_file(const char* file_path)
{
	FILE* f = fopen(file_path, "rb");
	if (!f)
	{
		printf("fopen failed");
		exit(EXIT_FAILURE);
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* string = malloc(fsize + 1);
	if(!string)
	{
		printf("malloc failed");
		exit(EXIT_FAILURE);
	}
	fread(string, fsize, 1, f);
	fclose(f);
	string[fsize] = 0;
	return string;
}
*/

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

	// generate the shader storage buffer object
	glGenBuffers(1, &_ssb);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssb);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * BOARD_SIZE * BOARD_SIZE * 2, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _ssb);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	// create the shader
	
	_shd = ogl_create_shader_program(vertex_shader_source, fragment_shader_source);
	
	// char* fragment_shader_source_T = read_file("shader.glsl");
	// _shd = ogl_create_shader_program(vertex_shader_source, fragment_shader_source_T);
	// free(fragment_shader_source_T);

	if (!_shd)
		return false;

	_shd_copy = ogl_create_shader_program(vertex_shader_source, fragment_shader_source_copy);
	if (!_shd_copy)
		return false;

	// load up the uniforms in advance
	uniforms.resolution = glGetUniformLocation(_shd, "_Resolution");
	uniforms.boardSize = glGetUniformLocation(_shd, "_BoardSize");
	uniforms.animationPercentage = glGetUniformLocation(_shd, "_AnimationPercentage");

	uniforms.resolution_copy = glGetUniformLocation(_shd_copy, "_Resolution");
	uniforms.frame_copy = glGetUniformLocation(_shd_copy, "_Frame");

	// we need this alternate framebuffer so that we can render the
	// actual game at a lower resolution for higher performance
	// and then streach and display it on the main window at a
	// higher resolution
	_fbo = ogl_create_framebuffer(GAME_FRAME_RESOLUTION, GAME_FRAME_RESOLUTION);

	_renderer_is_initiated = true;
	return _renderer_is_initiated;
}

void renderer_update_game_board(int* old_board, int* new_board)
{
	// update the game boards to the shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssb);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * BOARD_SIZE * BOARD_SIZE, old_board);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * BOARD_SIZE * BOARD_SIZE, sizeof(int) * BOARD_SIZE * BOARD_SIZE, new_board);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _ssb);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void renderer_render(float animation_percentage)
{
	// render the actual game on the alternate framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo.handle);
	glViewport(0, 0, GAME_FRAME_RESOLUTION, GAME_FRAME_RESOLUTION);

	// clear the screen with a light shade of grey
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// clear both the color and depth attachments
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind the shader
	glUseProgram(_shd);
	
	// update the uniforms
	glUniform2f(uniforms.resolution, (float)GAME_FRAME_RESOLUTION, (float)GAME_FRAME_RESOLUTION);
	glUniform1i(uniforms.boardSize, BOARD_SIZE);
	glUniform1f(uniforms.animationPercentage, animation_percentage);

	// bind the veertex array and render the quad
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



	// render the content of the alternate framebuffer onto the main
	// framebuffer at a higher resolution

	// bind the default framebuffer (not necessarily needed)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// out main viwport (the window) has fixed size of 512 x 512 pixels
	glViewport(0, 0, 512, 512);

	// clear the screen with a light shade of grey
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// clear both the color and depth attachments
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

	// bind copy shader
	glUseProgram(_shd_copy);
	// update the uniforms
	glUniform2f(uniforms.resolution_copy, 512.0f, 512.0);
	// find the alternate framebuffer's color texture
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, _fbo.color);
	glUniform1i(uniforms.frame_copy, 0);


	// bind the veertex array and render the quad
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void renderer_shutdown()
{
	if (!_renderer_is_initiated)
		return;

	// free the alternate framebuffer
	ogl_destroy_framebuffer(_fbo);

	// free the shader storage buffer object
	glDeleteBuffers(1, &_ssb);
	// free the vertex array
	glDeleteVertexArrays(1, &_vao);
	// free the vertex buffer
	glDeleteBuffers(1, &_vbo);
	// free the index buffer
	glDeleteBuffers(1, &_ebo);
	// free the shaders
	glDeleteShader(_shd);
	glDeleteShader(_shd_copy);
	
	_renderer_is_initiated = false;
}