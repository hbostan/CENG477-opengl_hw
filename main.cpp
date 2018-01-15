#include "helper.h"
#include <unistd.h>

#define PI 3.14159265

static GLFWwindow* win = NULL;
static boolean fullscreen = false;

typedef struct Camera {
  glm::vec3 position;
  glm::vec3 gaze;
  glm::vec3 right;
  glm::vec3 up;
  float fov;
  float aspectRatio;
  float near;
  float far;
  float speed;
} Camera;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int textureHeight, textureWidth;
int screenHeight, screenWidth;
float heightFactor = 10.0f;

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
  camera.fov = 45.0f;
  camera.aspectRatio = 1.0f;
  camera.near = 0.1f;
  camera.far = 1000.0f;
  camera.speed = 0.0f;
}

void setCamera() {
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::lookAt(camera.position, camera.position+ camera.gaze, camera.up);
  glm::mat4 perspective = glm::perspective(camera.fov, camera.aspectRatio, camera.near, camera.far);
  glm::mat4 mvp = perspective*view*model;
  GLuint matrixId = glGetUniformLocation(idProgramShader, "MVP");
  glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
}

void updateCameraVectors(int dir) {
  switch(dir) {
    case 0: // PITCH
      camera.gaze = glm::normalize(camera.gaze);
      camera.up = cross(-camera.right, camera.gaze);
      camera.right = cross(-camera.gaze, camera.up);
      setCamera();
      break;
    case 1: // YAW
      camera.gaze = glm::normalize(camera.gaze);
      camera.right = cross(-camera.gaze, camera.up);
      camera.up = cross(-camera.right, camera.gaze);
      setCamera();
      break;
    default:
      break;
  }
  std::cout << "Pos: "<< camera.position.x << ", "<< camera.position.y << ", "<<camera.position.z<<"\n";
  std::cout << "Gaze: "<< camera.gaze.x << ", "<< camera.gaze.y << ", "<<camera.gaze.z<<"\n";
  std::cout << "Up: "<< camera.up.x << ", "<< camera.up.y << ", "<<camera.up.z<<"\n--------------\n";

}

void rotateVector(float angle, glm::vec3& original, glm::vec3& direction)
{
	float m[16];

	angle = angle * PI / 180;
    float cosine = cos(angle);
	float sine = sin(angle);

	glm::vec3 u = glm::normalize(direction);//direction.normalized();
  
	float x = u.x;
	float y = u.y;
	float z = u.z;
	float t = 1 - cosine;

	m[0] = t * x * x + cosine;
	m[1] = t * x * y - sine * z;
	m[2] = t * x * z + sine * y;
	m[3] = 0.0;

	m[4] = t * x * y + sine * z;
	m[5] = t * y * y + cosine;
	m[6] = t * y * z - sine * x;
	m[7] = 0.0;

	m[8] = t * x * z - sine * y;
	m[9] = t * y * z + sine * x;
	m[10] = t * z * z + cosine;
	m[11] = 0.0;

	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;

	glm::vec3 result = glm::vec3(0);
    result.x = m[0] * original.x + m[1] * original.y + m[2] * original.z;
    result.y = m[4] * original.x + m[5] * original.y + m[6] * original.z;
    result.z = m[8] * original.x + m[9] * original.y + m[10] * original.z;
    original = result;

}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(win, GLFW_TRUE);

  // fullscreen
  else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    std::cout << "Fullscreen" << std::endl;
    if(!fullscreen){
      fullscreen = true;
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      glfwSetWindowMonitor(win, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else if (fullscreen) {
      fullscreen = false;
      glfwSetWindowMonitor(win, NULL, 100, 100, 600, 600, 0);
    }
    

  }

	// yaw
	else if (key == GLFW_KEY_A && action != GLFW_RELEASE) {//== GLFW_PRESS) {
    rotateVector(1.0, camera.gaze, camera.up);
    updateCameraVectors(1);
  }
	else if (key == GLFW_KEY_D && action != GLFW_RELEASE) {//== GLFW_PRESS) {
    rotateVector(-1.0, camera.gaze, camera.up);
    updateCameraVectors(1);
  }
		
	// pitch
	else if (key == GLFW_KEY_W && action != GLFW_RELEASE) {//== GLFW_PRESS) {
		rotateVector(1.0, camera.gaze, camera.right);
    updateCameraVectors(0);
  }
	else if (key == GLFW_KEY_S && action != GLFW_RELEASE) {//== GLFW_PRESS) {
    rotateVector(-1.0, camera.gaze, camera.right);
    updateCameraVectors(0);
  }

	else if (key == GLFW_KEY_U && action == GLFW_PRESS)
		camera.speed += 0.1;
	else if (key == GLFW_KEY_J && action == GLFW_PRESS)
		camera.speed -= 0.1;

	else if (key == GLFW_KEY_O && action == GLFW_PRESS)
		heightFactor += 0.5;
	else if (key == GLFW_KEY_L && action == GLFW_PRESS)
		heightFactor -= 0.5;

	else
		;
}

void fillVertices(int textureHeight, int textureWidth, GLfloat* vertexArray) {
  float widthChange = 1.0 / textureWidth;
  float heightChange = 1.0 / textureHeight;
  
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
      //std::cout << vertexArray[k-1] << "   ";
      vertexArray[k++] = 0.0f;  // y of bottom right
      vertexArray[k++] = (i+1)* heightChange;  // z of bottom right
      //std::cout << vertexArray[k-1] << std::endl;

      vertexArray[k++] = (j + 1)* widthChange;  // x of top right
      vertexArray[k++] = 0.0f;  // y of top right
      vertexArray[k++] = i* heightChange; // z of top right
      
    }
  }
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

  win = glfwCreateWindow(1024, 1024, "CENG477 - HW4", NULL, NULL);

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

  GLuint textureWidth_shader = glGetUniformLocation(idProgramShader, "widthTexture");
  GLuint textureHeight_shader = glGetUniformLocation(idProgramShader, "heightTexture");
  GLuint heightFactor_shader = glGetUniformLocation(idProgramShader, "heightFactor");
  GLuint cameraPosition_shader = glGetUniformLocation(idProgramShader, "cameraPosition");
  GLuint lightPosition_shader = glGetUniformLocation(idProgramShader, "lightPosition");

  initTexture(argv[1], &textureWidth, &textureHeight);
  std::cout << textureWidth<< " " << textureHeight << std::endl;  
  
  initCamera();
  GLfloat camPos[] = {camera.position.x, camera.position.y, camera.position.z};
  glUniform3fv(cameraPosition_shader, 1, camPos);//set camera pos

  glUniform1i(textureWidth_shader, textureWidth);  // Texture width
  glUniform1i(textureHeight_shader, textureHeight);// Texture height
  glUniform1f(heightFactor_shader, heightFactor); // Set height factor

  /////// NOT SURE IF EVEN NEEDED
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat lightColor[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat lightPosition[] = { textureWidth/2.0, textureWidth+textureHeight, textureHeight/2.0, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
  glUniform3fv(lightPosition_shader, 1, lightPosition);
  
  long triangleCount = textureWidth * textureHeight * 2;
  long vertexCount = triangleCount*3;
  long coordinateCount = vertexCount * 3;
  long vertexArraySizeInBytes = sizeof(GLfloat) * coordinateCount;
  GLfloat* vertexArray = new GLfloat[coordinateCount];
  std::cout << coordinateCount <<std::endl;
  fillVertices(textureHeight, textureWidth, vertexArray);

  GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertexArraySizeInBytes, vertexArray, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  setCamera();

  glfwSetKeyCallback(win, keyCallback);
  			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while(!glfwWindowShouldClose(win)) {
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1f(heightFactor_shader, heightFactor); // Set height factor

    camera.position = camera.position + (camera.gaze * camera.speed);
    setCamera();
    //std::cout << camera.position.x <<" "<< camera.position.y <<" "<<camera.position.z<<std::endl;
    GLfloat newPos[] = {camera.position.x, camera.position.y, camera.position.z};
    glUniform3fv(cameraPosition_shader, 1, newPos);//set camera pos

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


