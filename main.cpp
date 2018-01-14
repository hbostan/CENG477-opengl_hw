#include "helper.h"
#include <unistd.h>

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
  camera.position = glm::vec3(textureWidth/2.0, textureWidth/10.0, -textureWidth/4.0);
  std::cout << camera.position.x<<" "<<camera.position.y<<" "<<camera.position.z<<" " << std::endl;
  camera.gaze = glm::vec3(0, 0, 1);
  camera.up = glm::vec3(0, 1, 0);
  camera.right = glm::cross(-camera.gaze, camera.up);
  camera.fov = glm::radians(45.0f);
  camera.aspectRatio = 1.0f;
  camera.near = 0.1f;
  camera.far = 1000.0f;
}

void setCamera() {
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::lookAt(camera.position, camera.position+ camera.gaze, camera.up);
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
  std::cout << textureWidth<< " " << textureHeight << std::endl;  
  initCamera();

  GLuint widthTex = glGetUniformLocation(idProgramShader, "widthTexture");
	GLuint heightTex = glGetUniformLocation(idProgramShader, "heightTexture");
  

	glUniform1i(widthTex, textureWidth);
  glUniform1i(heightTex, textureHeight);
  
  

  long triangleCount = textureWidth * textureHeight * 2;
  long vertexCount = triangleCount*3;
  long coordinateCount = vertexCount * 3;
  long vertexArraySizeInBytes = sizeof(GLfloat) * coordinateCount;
  GLfloat* vertexArray = new GLfloat[coordinateCount];

  float widthChange = 1.0 / textureWidth;
  float heightChange = 1.0 / textureHeight;
  std::cout << coordinateCount <<std::endl;
  long k = 0;
  for(int i=0; i<textureHeight; i++) {
    for(int j=0; j<textureWidth; j++) {
      // First triangle
      vertexArray[k++] = (j + 1) * widthChange;  // x of top right
      vertexArray[k++] = 0.0f;  // y of top right
      vertexArray[k++] = i * heightChange; // z of top right

      vertexArray[k++] = j* widthChange;  // x of top left
      vertexArray[k++] = 0.0f;  // y of top left
      vertexArray[k++] = i* heightChange; // z of top left

      vertexArray[k++] = j* widthChange;  // x of bottom left
      vertexArray[k++] = 0.0f;  // y of bottom left
      vertexArray[k++] = (i + 1)* heightChange;  // z of bottom left

      // Second Triangle

      vertexArray[k++] = j* widthChange;  // x of bottom left
      vertexArray[k++] = 0.0f;  // y of bottom left
      vertexArray[k++] = (i + 1)* heightChange;  // z of bottom left

      vertexArray[k++] = (j + 1)* widthChange;  // x of bottom right
      vertexArray[k++] = 0.0f;  // y of bottom right
      vertexArray[k++] = (i+1)* heightChange;  // z of bottom right

      vertexArray[k++] = (j + 1)* widthChange;  // x of top right
      vertexArray[k++] = 0.0f;  // y of top right
      vertexArray[k++] = i* heightChange; // z of top right
    }
  }

  static const GLfloat g_vertex_buffer_data[] = {
		0, 0, 0,
		500,  0, 100,
		700,  0, 100,
	};


  GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, vertexArraySizeInBytes, vertexArray, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  setCamera();
  while(!glfwWindowShouldClose(win)) {
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glDisableVertexAttribArray(0);
    glfwSwapBuffers(win);
    glfwPollEvents();
    
  
  }


  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}
