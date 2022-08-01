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


// dows nothing now maybe for future
void ogl_shutdown()
{
}
