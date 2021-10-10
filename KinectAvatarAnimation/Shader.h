#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <Windows.h>
#include <gl/glew.h>

#include <string>
#include <fstream>
#include <stdexcept>

std::string getFileContent(const char* filename) throw (std::runtime_error);

class Shader
{
public:
	GLuint ID;
	Shader(const char* vertexFile, const char* fragmentFile)  throw (std::runtime_error);
	~Shader();
	void activate();
	

private:
	void compileErrors(unsigned int shader, const char* type) throw (std::runtime_error);
};

#endif