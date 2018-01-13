#include "helper.h"

static GLFWwindow* win = NULL;

typedef struct Camera {
  glm::vec3 position;
  glm::vec3 gaze;
  glm::vec3 right;
  glm::vec3 up;
  float fov;
  float aspectRatio;
  float near;
  float far;
} Camera;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int textureHeight, textureWidth;
int screenHeight, screenWidth;

Camera camera;

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void initCamera() {
  camera.position = glm::vec3(textureWidth/2, textureWidth/10, -textureWidth/4);
  camera.gaze = glm::vec3(0, 0, 1);
  camera.up = glm::vec3(0, 1, 0);
  camera.right = glm::cross(camera.gaze, camera.up);
  camera.fov = 45.0f;
  camera.aspectRatio = 1.0f;
  camera.near = 0.1f;
  camera.far = 1000.0f;
}

void setCamera() {
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::lookAt(camera.position, camera.gaze, camera.up);
  glm::mat4 perspective = glm::perspective(camera.fov, camera.aspectRatio, camera.near, camera.far);
  glm::mat4 mvp = perspective*view*model;
  GLuint matrixId = glGetUniformLocation(idProgramShader, "MVP");
  glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Please provide only a texture image\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(600, 600, "CENG477 - HW4", NULL, NULL);

  if (!win) {
      glfwTerminate();
      exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

      glfwTerminate();
      exit(-1);
  }

  

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], &textureWidth, &textureHeight);

  initCamera();

  static const GLfloat g_vertex_buffer_data[] = {
		-100.0f, 0, 100.0f,
		100.0f,  0, 100.0f,
		0.0f,  0,  300.0f,
	};


  GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  setCamera();

  glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  while(!glfwWindowShouldClose(win)) {
    
    glfwSwapBuffers(win);
    glfwPollEvents();
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glEnableVertexAttribArray(0);
  
  }


  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}
