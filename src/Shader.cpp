#include "Shader.h"

// немного магии :)
#define DEFINE_TO_STR_HELPER(x) #x
#define DEFINE_TO_STR(x)        "#define " #x " " DEFINE_TO_STR_HELPER(x) "\n"

// набор параметров для вершинного шейдера
static const char vertexShaderDefines[] =
	"#version 330 core\n"
	DEFINE_TO_STR(VERT_POSITION)
	DEFINE_TO_STR(VERT_TEXCOORD)
	DEFINE_TO_STR(VERT_NORMAL)
	"\n";

// набор параметров для фрагментного шейдера
static const char fragmentShaderDefines[] =
	"#version 330 core\n"
	DEFINE_TO_STR(FRAG_OUTPUT0)
	"\n";


// проверка статуса шейдерной программы
GLint ShaderProgramStatus(GLuint program, GLenum param)
{
	ASSERT(program);

	GLint status, length;
	GLchar buffer[1024];

	glGetProgramiv(program, param, &status);

	if (status != GL_TRUE)
	{
		glGetProgramInfoLog(program, 1024, &length, buffer);
		LOG_ERROR("Shader program: %s\n", (const char*)buffer);
	}

	OPENGL_CHECK_FOR_ERRORS();

	return status;
}

// проверка статуса шейдера
GLint ShaderStatus(GLuint shader, GLenum param)
{
	ASSERT(shader);

	GLint status, length;
	GLchar buffer[1024];

	glGetShaderiv(shader, param, &status);

	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(shader, 1024, &length, buffer);
		LOG_ERROR("Shader: %s\n", (const char*)buffer);
	}

	OPENGL_CHECK_FOR_ERRORS();

	return status;
}

GLuint ShaderProgramCreateFromFile(const char *fileName, int type)
{
	ASSERT(fileName);
	ASSERT(type);

	GLuint   program, shader;
	uint8_t  *shaderSource;
	uint32_t sourceLength;
	char     shaderName[0x100];

	// слздадим шейдерную программу
	if ((program = glCreateProgram()) == 0)
	{
		LOG_ERROR("Creating shader program fail (%d)\n", glGetError());
		return 0;
	}

	memset(shaderName, 0, 0x100);

	// если необходимо создать вершинный шейдер
	if (type & ST_VERTEX)
	{
		// создадим вершинный шейдер
		if ((shader = glCreateShader(GL_VERTEX_SHADER)) == 0)
		{
			LOG_ERROR("Creating vertex shader fail (%d)\n", glGetError());
			glDeleteProgram(program);
			return 0;
		}

		// имя вершинного шейдера
		snprintf(shaderName, 0xFF, "%s.vs", fileName);

		// загрузим вершинный шейдер
		if (!LoadFile(shaderName, true, &shaderSource, &sourceLength))
		{
			glDeleteShader(shader);
			glDeleteProgram(program);
			return 0;
		}

		// добавим к коду вершинного шейдера параметры
		const GLchar *source[2] = {(const GLchar*)vertexShaderDefines, (const GLchar*)shaderSource};
		const GLint  length[2] = {sizeof(vertexShaderDefines) - 1, sourceLength};

		// зададим шейдеру исходный код и скомпилируем его
		glShaderSource(shader, 2, source, length);
		glCompileShader(shader);

		delete[] shaderSource;

		// проверим статус компиляции шейдера
		if (ShaderStatus(shader, GL_COMPILE_STATUS) != GL_TRUE)
		{
			glDeleteShader(shader);
			glDeleteProgram(program);
			return 0;
		}

		// присоеденим загруженные шейдеры к шейдерной программе
		glAttachShader(program, shader);

		// шейдер нам больше не нужен, пометим его на удаление
		// он будет удален вместе с шейдерной программой
		glDeleteShader(shader);
	}

	// если необходимо создать фрагментный шейдер
	if (type & ST_FRAGMENT)
	{
		// создадим вершинный шейдер
		if ((shader = glCreateShader(GL_FRAGMENT_SHADER)) == 0)
		{
			LOG_ERROR("Creating fragment shader fail (%d)\n", glGetError());
			glDeleteProgram(program);
			return 0;
		}

		// имя фрагментного шейдера
		snprintf(shaderName, 0xFF, "%s.fs", fileName);

		// загрузим фрагментный шейдер
		if (!LoadFile(shaderName, true, &shaderSource, &sourceLength))
		{
			glDeleteShader(shader);
			glDeleteProgram(program);
			return 0;
		}

		// добавим к коду фрагментного шейдера параметры
		const GLchar *source[2] = {(const GLchar*)fragmentShaderDefines, (const GLchar*)shaderSource};
		const GLint  length[2] = {sizeof(fragmentShaderDefines) - 1, sourceLength};

		// зададим шейдеру исходный код и скомпилируем его
		glShaderSource(shader, 2, source, length);
		glCompileShader(shader);

		delete[] shaderSource;

		// проверим статус компиляции шейдера
		if (ShaderStatus(shader, GL_COMPILE_STATUS) != GL_TRUE)
		{
			glDeleteShader(shader);
			glDeleteProgram(program);
			return 0;
		}

		// присоеденим загруженные шейдеры к шейдерной программе
		glAttachShader(program, shader);

		// шейдер нам больше не нужен, пометим его на удаление
		// он будет удален вместе с шейдерной программой
		glDeleteShader(shader);
	}

	OPENGL_CHECK_FOR_ERRORS();

	return program;
}

void ShaderProgramDestroy(GLuint program)
{
	ShaderProgramUnbind();
	glDeleteProgram(program);
}

bool ShaderProgramLink(GLuint program)
{
	// линковка шейдерной программы и проверка статуса линковки
	glLinkProgram(program);
	return (ShaderProgramStatus(program, GL_LINK_STATUS) == GL_TRUE);
}

bool ShaderProgramValidate(GLuint program)
{
	// проверка на корректность шейдерной программы
	glValidateProgram(program);
	return (ShaderProgramStatus(program, GL_VALIDATE_STATUS) == GL_TRUE);
}

void ShaderProgramBind(GLuint program)
{
	// сделаем шейдерную программу активной
	glUseProgram(program);
}

void ShaderProgramUnbind()
{
	// сделаем текущую шейдерную программу неактивной
	glUseProgram(0);
}
