#include "Shader.h"

std::string getFileContent(const char* filename) throw (std::runtime_error)
{
	std::ifstream fileSource(filename);
	if (!fileSource) {
		throw std::runtime_error("failed to open ifstream");
	}
	std::string file((std::istreambuf_iterator<char>(fileSource)), (std::istreambuf_iterator<char>()));

	return file;
}
/*
* @constructor Get shaders from file, compiling
*				and linking them to the openGL program.
*				If everything is ok, then it actives the program.
* @throw runtime_error if shader program creating goes wrong
*/
Shader::Shader(const char* vertexFile, const char* fragmentFile) throw (std::runtime_error)
{
	// get and set shaders from file
	std::string vs = getFileContent(vertexFile);
	std::string fs = getFileContent(fragmentFile);

	const char* vsc = vs.c_str();
	const char* fsc = fs.c_str();

	// create and compile shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsc, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsc, NULL);
	glCompileShader(fragmentShader);

	// create openGL program
	ID = glCreateProgram();

	// link compiled shaders
	GLenum errorCode;
	glAttachShader(ID, vertexShader);
	errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
		throw std::runtime_error("Ver shader error: " + errorCode);
	
	glAttachShader(ID, fragmentShader);
	errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
		throw std::runtime_error("Frag shader error: " + errorCode);

	glLinkProgram(ID);

	// delete the now useless vertex and fragment shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// check for errors
	GLint params;
	glGetProgramiv(ID, GL_LINK_STATUS, &params);
	if (params != GL_TRUE) {
		GLchar errorLog[1024] = { 0 };
		glGetProgramInfoLog(ID, 1024, NULL, errorLog);
		throw std::runtime_error(("error validating shader program; Details: %s", errorLog));
	}
}

void Shader::activate() 
{
	// activate openGL program
	glUseProgram(ID);
}

void Shader::deactivate()
{
	// delete openGL program
	glDeleteProgram(ID);
}