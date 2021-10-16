#include "Shader.h"

std::string getFileContent(const char* filename) throw (std::runtime_error)
{
	std::ifstream fileSource(filename);
	if (!fileSource) {
		throw std::runtime_error("Failed to open ifstream \n");
	}
	std::string file((std::istreambuf_iterator<char>(fileSource)), (std::istreambuf_iterator<char>()));

	return file;
}
/**
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
	glAttachShader(ID, vertexShader);
	compileErrors(vertexShader, "VERTEX");
	
	glAttachShader(ID, fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	glLinkProgram(ID);
	compileErrors(ID, "PROGRAM");

	// delete the now useless vertex and fragment shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::activate() 
{
	// activate openGL program
	glUseProgram(ID);
}

GLuint Shader::getUniformLocation(const char* uniform) throw (std::runtime_error)
{
	GLuint u_index = glGetUniformLocation(ID, uniform);
	if (u_index == -1) {
		throw std::runtime_error(("Failed to load %s.", uniform));
	}
	return u_index;
}

Shader::~Shader()
{
	// delete openGL program
	glDeleteProgram(ID);
}

/**
* @function check for errors occured in shader initialization
* @params shader is ID generated from an openGL shader or program
* @params type is "PROGRAM", "VERTEX" or "FRAGMENT"
* @thow runtime_error if error is discovered
*/
void Shader::compileErrors(unsigned int shader, const char* type) throw (std::runtime_error)
{
	GLint params;
	GLchar errorLog[1024] = { 0 };

	if (std::strcmp("PROGRAM", type) == 0) {
		glGetProgramiv(shader, GL_LINK_STATUS, &params);
		if (params != GL_TRUE) {
			glGetProgramInfoLog(shader, 1024, NULL, errorLog);
			throw std::runtime_error(("SHADER_LINKING_ERROR for %s. Details: %s", type, errorLog));
		}
	} else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
		if (params != GL_TRUE) {
			glGetShaderInfoLog(shader, 1024, NULL, errorLog);
			throw std::runtime_error(("SHADER_COMPILATION_ERROR for %s. Details: %s", type, errorLog));
		}
	}
}