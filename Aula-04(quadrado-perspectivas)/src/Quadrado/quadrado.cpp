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
const GLfloat kPerspectiveTranslateZ = 10;

bool translating = false;
const GLfloat kTranslateLimit = 10;
const GLfloat kDefaultTranslate = 0;
GLfloat translate_increment = 0.01;
GLfloat translate = kDefaultTranslate;
GLfloat translate_x = 0.0f;
GLfloat translate_y = 0.0f;
GLfloat translate_z = 0.0f;

const GLfloat kRotateAngleLimit = 180;
const GLfloat kDefaultRotateAngle = 0;
GLfloat rotate_angle_increment = 0.3;
GLfloat rotate_angle = kDefaultRotateAngle;
GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;

bool scaling = false;
const GLfloat kScaleLimit = 10;
const GLfloat kDefaultScale = 1;
GLfloat scale_increment = 0.01;
GLfloat scale = kDefaultScale;
GLfloat scale_x = 1.0f;
GLfloat scale_y = 1.0f;
GLfloat scale_z = 1.0f;

void keyboard_read(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    translate = kDefaultTranslate;
    rotate_x = 0.0f;
    rotate_y = 0.0f;
    rotate_z = 0.0f;
    translate_x = 0.0f;
    translate_y = 0.0f;
    translate_z = 0.0f;
    scale = kDefaultScale;
    scale_x = 1.0f;
    scale_y = 1.0f;
    scale_z = 1.0f;
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
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      translate_x += translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      translate_x -= translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      translate_y += translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      translate_y -= translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
      translate_z += translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
      translate_z -= translate_increment;
    }
  }

  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      rotate_y += rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      rotate_y -= rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      rotate_x += rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      rotate_x -= rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
      rotate_z += rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
      rotate_z -= rotate_angle_increment;
    }
  }

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      scale_x += scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      scale_x -= scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      scale_y += scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      scale_y -= scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
      scale_z += scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
      scale_z -= scale_increment;
    }
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
    glfwSetWindowTitle(window, "Tetraedro ortogonal");
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
    glfwSetWindowTitle(window, "Tetraedro em perspectiva");
    GLdouble field_of_view = kPerspectiveFieldOfViewAngle;
    GLdouble near = kPerspectiveNearZ;
    GLdouble far = kPerspectiveFarZ;
    gluPerspective(field_of_view, aspect_ratio, near, far);
  }
}

void draw() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (perspective_view) {
    glTranslatef(translate_x, translate_y, translate_z - kPerspectiveTranslateZ);
  } else {
    glTranslatef(translate_x, translate_y, translate_z);
  }

  glRotatef(rotate_y, 0, 1, 0);
  glRotatef(rotate_x, 1, 0, 0);
  glRotatef(rotate_z, 0, 0, 1);

  glScalef(scale_x, scale_y, scale_z);

  glBegin(GL_QUADS);
  {
    glColor3ub(255, 255, 255);
    glVertex3f(-2, -2, 0);
    glVertex3f(2, -2, 0);
    glVertex3f(2, 2, 0);
    glVertex3f(-2, 2, 0);
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