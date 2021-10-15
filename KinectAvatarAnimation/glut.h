#pragma once
#include <Windows.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>

void init(int argc, char* argv[]);
void draw();
void mouse(int button, int state, int x, int y);
void movingMouse(int x, int y);
void execute();