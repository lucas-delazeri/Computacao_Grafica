// Copyright
// Computação Gráfica
// URI Santiago
// Professor Laurence

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <iostream>

bool orthographic_view = true;
const GLfloat kOrthographicLimitX = 5;
const GLfloat kOrthographicLimitY = 5;
const GLfloat kOrthographicLimitZ = 20;

bool perspective_view = false;
const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1;
const GLfloat kPerspectiveFarZ = 100;
const GLfloat kPerspectiveTranslateZ = 12;

bool translating = false;
const GLfloat kTranslateLimit = 10;
const GLfloat kDefaultTranslate = 0;
GLfloat translate_increment = 0.1;
GLfloat translate = kDefaultTranslate;

bool rotating = false;
const GLfloat kRotateAngleLimit = 180;
const GLfloat kDefaultRotateAngle = 0;
GLfloat rotate_angle_increment = 1;
GLfloat rotate_angle = kDefaultRotateAngle;

bool scaling = false;
const GLfloat kScaleLimit = 10;
const GLfloat kDefaultScale = 1;
GLfloat scale_increment = 0.1;
GLfloat scale = kDefaultScale;

void keyboard_read(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    translate = kDefaultTranslate;
    rotate_angle = kDefaultRotateAngle;
    scale = kDefaultScale;
  }

  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
    orthographic_view = true;
    perspective_view = false;
  }

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    orthographic_view = false;
    perspective_view = true;
  }

  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    translating = true;
  }
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
    translating = false;
  }

  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    rotating = true;
  }
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
    rotating = false;
  }

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    scaling = true;
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
    scaling = false;
  }
}

void resize_window(GLFWwindow* window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);

  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (orthographic_view) {
    glfwSetWindowTitle(window, "Triangulo ortogonal");
    GLdouble left = -kOrthographicLimitX;
    GLdouble right = kOrthographicLimitX;
    GLdouble bottom = -kOrthographicLimitY;
    GLdouble top = kOrthographicLimitY;
    GLdouble near = -kOrthographicLimitZ;
    GLdouble far = kOrthographicLimitZ;
    if (window_width > window_height) {
      glOrtho((left * aspect_ratio), (right * aspect_ratio), bottom, top, near, far);
    } else {
      glOrtho(left, right, (bottom / aspect_ratio), (top / aspect_ratio), near, far);
    }
  }
  if (perspective_view) {
    glfwSetWindowTitle(window, "Triangulo em perspectiva");
    GLdouble field_of_view = kPerspectiveFieldOfViewAngle;
    GLdouble near = kPerspectiveNearZ;
    GLdouble far = kPerspectiveFarZ;
    gluPerspective(field_of_view, aspect_ratio, near, far);
  }
}

void draw() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(translate, 0, -kPerspectiveTranslateZ);
  if (translating) {
    translate += translate_increment;
    if ((translate <= -kTranslateLimit) || (translate >= kTranslateLimit)) {
      translate_increment *= -1;
    }
  }

  glRotatef(rotate_angle, 1, 0, 0);
  if (rotating) {
    rotate_angle += rotate_angle_increment;
    if ((rotate_angle <= -kRotateAngleLimit) || (rotate_angle >= kRotateAngleLimit)) {
      rotate_angle_increment *= -1;
    }
  }

  glScalef(scale, 1, 1);
  if (scaling) {
    scale += scale_increment;
    if ((scale <= -kScaleLimit) || (scale >= kScaleLimit)) {
      scale_increment *= -1;
    }
  }

  glBegin(GL_TRIANGLES);
  {
    glColor3ub(255, 0, 0);
    glVertex2f(0, 2.5);
    glColor3ub(0, 255, 0);
    glVertex2f(-2.5, -2.5);
    glColor3ub(0, 0, 255);
    glVertex2f(2.5, -2.5);
  }
  glEnd();
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  GLFWwindow* window = glfwCreateWindow(800, 800, "", NULL, NULL);
  if (!window) {
    std::cerr << "Falha ao criar a janela GLFW" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwSetWindowPos(window, 0, 0);
  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    resize_window(window);
    draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}