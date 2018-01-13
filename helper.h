#ifndef __HELPER__H__
#define __HELPER__H__

#include <iostream>
#include <string>
#include <fstream>
#include <jpeglib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

extern GLuint idProgramShader;
extern GLuint idFragmentShader;
extern GLuint idVertexShader;
extern GLuint idJpegTexture;

using namespace std;

void initShaders();

GLuint initVertexShader(const string& filename);

GLuint initFragmentShader(const string& filename);

bool readDataFromFile(const string& fileName, string &data);

void initTexture(char *filename,int *w, int *h);

#endif
