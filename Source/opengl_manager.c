#include "opengl_manager.h"

bool _ogl_is_initiated = false;

// loads the opengl functions pointers using glad
bool ogl_init()
{
	// check is glad is already initated
	if (_ogl_is_initiated)
		return true;

	// load opengl functions
	_ogl_is_initiated = gladLoadGL();
	
	// is failed print error log
	if (!_ogl_is_initiated)
		printf("Failed to initiate ogl\n");

	return _ogl_is_initiated;
}

uint32_t ogl_create_shader(const char* sc, GLenum type)
{
	GLuint shader = glCreateShader(type);
	const GLchar* source = (const GLchar*)sc;
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char* errorLog = malloc(sizeof(char) * maxLength);
		if (!errorLog)
		{
			printf("malloc failed");
			exit(EXIT_FAILURE);
		}
		errorLog[0] = 0;
		glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
		printf("Error in Compiling Shader : \n");
		printf("%s\n", errorLog);
		free(errorLog);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

uint32_t ogl_create_shader_program(const char* vertex_shader_source, const char* fragment_shader_source)
{
	uint32_t vertex_shader = ogl_create_shader(vertex_shader_source, GL_VERTEX_SHADER);
	uint32_t fragment_shader = ogl_create_shader(fragment_shader_source, GL_FRAGMENT_SHADER);
	if (!fragment_shader || !vertex_shader)
		return 0;
	uint32_t shader = glCreateProgram();
	glAttachShader(shader, vertex_shader);
	glAttachShader(shader, fragment_shader);
	glLinkProgram(shader);
	GLint isLinked = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, (int*)&isLinked);

	if (!isLinked)
	{
		GLint maxLength = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char* errorLog = malloc(sizeof(char) * maxLength);
		if (!errorLog)
		{
			printf("malloc failed");
			exit(EXIT_FAILURE);
		}
		errorLog[0] = 0;
		glGetProgramInfoLog(shader, maxLength, &maxLength, errorLog);
		printf("Error in Compiling Shader : ");
		printf("%s\n", errorLog);
		free(errorLog);
		return false;
	}
	glDetachShader(shader, vertex_shader);
	glDetachShader(shader, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return shader;
}

uint32_t ogl_create_texture(uint64_t width, uint64_t height, GLenum format, GLenum internal_format, GLenum data_type)
{
	uint32_t tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, data_type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

ogl_framebuffer_t ogl_create_framebuffer(uint64_t width, uint64_t height)
{
	ogl_framebuffer_t fbo;
	glGenFramebuffers(1, &fbo.handle);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.handle);
	fbo.color = ogl_create_texture(width, height, GL_RGBA, GL_RGBA, GL_FLOAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.color, 0);
	fbo.depth = ogl_create_texture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo.depth, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return fbo;
}


void ogl_destroy_framebuffer(ogl_framebuffer_t fbo)
{
	glDeleteTextures(1, &fbo.color);
	glDeleteTextures(1, &fbo.depth);
	glDeleteFramebuffers(1, &fbo.handle);
}

// dows nothing now maybe for future
void ogl_shutdown()
{
}
