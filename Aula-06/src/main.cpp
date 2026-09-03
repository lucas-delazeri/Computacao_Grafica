// Copyright
// Computação Gráfica
// URI Santiago
// Professor Laurence

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include "font.h"

bool orthographic_view = true;
const GLfloat kOrthographicLimitX = 5;
const GLfloat kOrthographicLimitY = 5;
const GLfloat kOrthographicLimitZ = 20;

bool perspective_view = false;
const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1;
const GLfloat kPerspectiveFarZ = 100;
const GLfloat kPerspectiveTranslateZ = 14;

bool translating = false;
const GLfloat kTranslateLimit = 2.5;
const GLfloat kDefaultTranslate = 0;
const GLfloat kTranslateIncrement = 0.01;
GLfloat translate_x = kDefaultTranslate;
GLfloat translate_y = kDefaultTranslate;
GLfloat translate_z = kDefaultTranslate;

bool rotating = false;
const GLfloat kRotateAngleLimit = 180;
const GLfloat kDefaultRotateAngle = 0;
const GLfloat kRotateAngleIncrement = 0.3;
GLfloat rotate_angle_x = kDefaultRotateAngle;
GLfloat rotate_angle_y = kDefaultRotateAngle;
GLfloat rotate_angle_z = kDefaultRotateAngle;

bool scaling = false;
const GLfloat kScaleLimit = 2.5;
const GLfloat kDefaultScale = 1;
const GLfloat kScaleIncrement = 0.01;
GLfloat scale_x = kDefaultScale;
GLfloat scale_y = kDefaultScale;
GLfloat scale_z = kDefaultScale;

GLfloat clock_hand_rotate_angle = 0;

// configs iniciais do HUD
const GLfloat kHudHeight = 300;
const GLfloat kHudWidth = 300;
std::ostringstream hud_text;

void keyboard_read(GLFWwindow* window) {
  hud_text.str(""); // inicializa o hud

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
    hud_text << "Fechando a janela..." << std::endl;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    translate_x = kDefaultTranslate;
    translate_y = kDefaultTranslate;
    translate_z = kDefaultTranslate;

    rotate_angle_x = kDefaultRotateAngle;
    rotate_angle_y = kDefaultRotateAngle;
    rotate_angle_z = kDefaultRotateAngle;

    scale_x = kDefaultScale;
    scale_y = kDefaultScale;
    scale_z = kDefaultScale;
    
  }

  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
    orthographic_view = true;
    perspective_view = false;

    hud_text << "Visualizacao Ortografica" << std::endl;
  }

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    orthographic_view = false;
    perspective_view = true;

    hud_text << "Visualizacao em Perspectiva" << std::endl;
  }

  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    translating = true;
    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
      if ((translate_x > -kTranslateLimit)) {
        translate_x -= kTranslateIncrement;

        hud_text << "Transladando para esquerda " << std::endl;
      }
    }
    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
      if ((translate_x < kTranslateLimit)) {
        translate_x += kTranslateIncrement;

        hud_text << "Transladando para direita" << std::endl;
      }
    }
    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
      if ((translate_y > -kTranslateLimit)) {
        translate_y -= kTranslateIncrement;

        hud_text << "Transladando para baixo" << std::endl;
      }
    }
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
      if ((translate_y < kTranslateLimit)) {
        translate_y += kTranslateIncrement;

        hud_text << "Transladando para cima" << std::endl;
      }
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)) {
      if ((translate_z > -kTranslateLimit)) {
        translate_z -= kTranslateIncrement;

        hud_text << "Transladando eixo z para tras" << std::endl;
      }
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)) {
      if ((translate_z < kTranslateLimit)) {
        translate_z += kTranslateIncrement;

        hud_text << "Transladando eixo z para frente" << std::endl;
      }
    }
  }

  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
    translating = false;
  }

  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    rotating = true;
    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
        rotate_angle_x += kRotateAngleIncrement;

        hud_text << "Rotacionando para baixo " << std::endl;
    
    }
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
        rotate_angle_x -= kRotateAngleIncrement;

        hud_text << "Rotacionando para cima " << std::endl;
    }
    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
        rotate_angle_y -= kRotateAngleIncrement;

        hud_text << "Rotacionando para esquerda " << std::endl;
    }
    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
        rotate_angle_y += kRotateAngleIncrement;

        hud_text << "Rotacionando para direita " << std::endl;
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)) {
        rotate_angle_z -= kRotateAngleIncrement;

        hud_text << "Rotacionando eixo z para baixo " << std::endl;
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)) {
        rotate_angle_z += kRotateAngleIncrement;

        hud_text << "Rotacionando eixo z para cima " << std::endl;
    }
  }

  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
    rotating = false;
  }

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    scaling = true;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (scale_x > 0.1f) {
            scale_x -= kScaleIncrement;
            hud_text << "Escalando X para esquerda" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (scale_x < kScaleLimit) {
            scale_x += kScaleIncrement;
            hud_text << "Escalando X para direita" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (scale_y > 0.1f) {
            scale_y -= kScaleIncrement;
            hud_text << "Escalando Y para baixo" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (scale_y < kScaleLimit) {
            scale_y += kScaleIncrement;
            hud_text << "Escalando Y para cima" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        if (scale_z > 0.1f) {
            scale_z -= kScaleIncrement;
            hud_text << "Escalando Z para frente" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        if (scale_z < kScaleLimit) {
            scale_z += kScaleIncrement;
            hud_text << "Escalando Z para trás" << std::endl;
        }
    }
}

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
    scaling = false;
  }
}

void resize_window(GLFWwindow* window) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  GLdouble aspect_ratio = (GLdouble)width / height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (orthographic_view) {
    glfwSetWindowTitle(window, "Cubo ortogonal");
    GLdouble left = -kOrthographicLimitX;
    GLdouble right = kOrthographicLimitX;
    GLdouble bottom = -kOrthographicLimitY;
    GLdouble top = kOrthographicLimitY;
    GLdouble near = -kOrthographicLimitZ;
    GLdouble far = kOrthographicLimitZ;
    if (width > height) {
      glOrtho((left * aspect_ratio), (right * aspect_ratio), bottom, top, near, far);
    } else {
      glOrtho(left, right, (bottom / aspect_ratio), (top / aspect_ratio), near, far);
    }
  }
  if (perspective_view) {
    glfwSetWindowTitle(window, "Cubo em perspectiva");
    GLdouble field_of_view = kPerspectiveFieldOfViewAngle;
    GLdouble near = kPerspectiveNearZ;
    GLdouble far = kPerspectiveFarZ;
    gluPerspective(field_of_view, aspect_ratio, near, far);
  }
}

void draw() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(translate_x, translate_y, (translate_z - kPerspectiveTranslateZ));

  glRotatef(rotate_angle_x, 1, 0, 0);
  glRotatef(rotate_angle_y, 0, 1, 0);
  glRotatef(rotate_angle_z, 0, 0, 1);

  glScalef(scale_x, scale_y, scale_z);

  
    // face frontal
    glPushMatrix();
    {
      glBegin(GL_QUADS);
      glColor3ub(255, 0, 0);
      glVertex3f(-2, -2, 2);
      glVertex3f(2, -2, 2);
      glVertex3f(2, 2, 2);
      glVertex3f(-2, 2, 2);

      glLineWidth(5);
      glColor3ub(255, 0, 255);

      glRotatef(clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      {
        glVertex3f(0, 0, 2.05); // vertices eixo z modificados para os ponteiros ficarem na frente do cubo
        glVertex3f(0.25, 0, 2.05);
      }
      glEnd();
      glPopMatrix();

      glPushMatrix();
      glRotatef(2 * clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 2.05);
      glVertex3f(0.5, 0, 2.05);
      glEnd();
      glPopMatrix();
    }
    glPopMatrix(); // desempilha e restaura a matriz

    // face traseira
    glPushMatrix();
    {
      glBegin(GL_QUADS);
      glColor3ub(0, 255, 0);
      glVertex3f(-2, -2, -2);
      glVertex3f(-2, 2, -2);
      glVertex3f(2, 2, -2);
      glVertex3f(2, -2, -2);
      glEnd();
      
      glRotatef(180, 0, 1, 0);

      glLineWidth(5);
      glColor3ub(255, 0, 255);

      glPushMatrix();
      glRotatef(clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 2.05);
      glVertex3f(0.25, 0, 2.05);
      glEnd();
      glPopMatrix();

      glPushMatrix();
      glRotatef(2 * clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 2.05);
      glVertex3f(0.5, 0, 2.05);
      glEnd();
      glPopMatrix();
    }
    glPopMatrix();

    // face esquerda = azul
    glPushMatrix();
    {
      glBegin(GL_QUADS);
      glColor3ub(0, 0, 255);
      glVertex3f(-2, -2, -2);
      glVertex3f(-2, -2, 2);
      glVertex3f(-2, 2, 2);
      glVertex3f(-2, 2, -2);
      glEnd();
      
      // Roda para projetar na face esquerda
      glTranslatef(-2.05, 0, 0);
      glRotatef(90, 0, 1, 0);

      glLineWidth(5);
      glColor3ub(255, 0, 255);

      glPushMatrix();
      glRotatef(clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 0);
      glVertex3f(0.25, 0, 0);
      glEnd();
      glPopMatrix();

      glPushMatrix();
      glRotatef(2 * clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 0);
      glVertex3f(0.5, 0, 0);
      glEnd();
      glPopMatrix();
    }
    glPopMatrix();

    // face direita = amarela
    glPushMatrix();
    {
      glBegin(GL_QUADS);
      glColor3ub(255, 255, 0);
      glVertex3f(2, -2, -2);
      glVertex3f(2, 2, -2);
      glVertex3f(2, 2, 2);
      glVertex3f(2, -2, 2);
      glEnd();

      // Roda para projetar na face direita
      glTranslatef(2.05, 0, 0);
      glRotatef(-90, 0, 1, 0);

      glLineWidth(5);
      glColor3ub(255, 0, 255);

      glPushMatrix();
      glRotatef(clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 0);
      glVertex3f(0.25, 0, 0);
      glEnd();
      glPopMatrix();

      glPushMatrix();
      glRotatef(2 * clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 0);
      glVertex3f(0.5, 0, 0);
      glEnd();
      glPopMatrix();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glBegin(GL_QUADS);
      glColor3ub(255, 0, 255);
      glVertex3f(-2, 2, -2);
      glVertex3f(-2, 2, 2);
      glVertex3f(2, 2, 2);
      glVertex3f(2, 2, -2);
      
      glLineWidth(5);
      glColor3ub(255, 0, 255);

      glRotatef(clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      {
        glVertex3f(0, 0, 2.05); // vertices eixo z modificados para os ponteiros ficarem na frente do cubo
        glVertex3f(0.25, 0, 2.05);
      }
      glEnd();

      glRotatef(2 * clock_hand_rotate_angle, 0, 0, 1);
      glBegin(GL_LINES);
      {
        glVertex3f(0, 0, 2.05);
        glVertex3f(0.5, 0, 2.05);
      }
      glEnd();

      clock_hand_rotate_angle--;
    }
    glPopMatrix();

    // 5. FACE SUPERIOR (Magenta) - Sem ponteiros
    glBegin(GL_QUADS);
    glColor3ub(255, 0, 255);
    glVertex3f(-2, 2, -2);
    glVertex3f(-2, 2, 2);
    glVertex3f(2, 2, 2);
    glVertex3f(2, 2, -2);
    glEnd();

    // 6. FACE INFERIOR (Ciano) - Sem ponteiros
    glBegin(GL_QUADS);
    glColor3ub(0, 255, 255);
    glVertex3f(-2, -2, -2);
    glVertex3f(2, -2, -2);
    glVertex3f(2, -2, 2);
    glVertex3f(-2, -2, 2);
    glEnd();

    clock_hand_rotate_angle--;
  }

void draw_hud(GLFWwindow* window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLdouble left = 0;
  GLdouble right = kHudWidth;
  GLdouble bottom = 0;
  GLdouble top = kHudHeight;
  if (window_width > window_height) {
    gluOrtho2D((left * aspect_ratio), (right * aspect_ratio), bottom, top);
  } else {
    gluOrtho2D(left, right, (bottom / aspect_ratio), (top / aspect_ratio));
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  {
    glColor3ub(255, 255, 255);
    draw_text(5, 5, hud_text.str());

    glPointSize(20);
    glBegin(GL_POINTS);
    {
      if (translating) {
        glColor3ub(255, 0, 0);
        glVertex2f(7.5, 20);
      }

      if (rotating) {
        glColor3ub(0, 255, 0);
        glVertex2f(17.5, 20);
      }

      if (scaling) {
        glColor3ub(0, 0, 255);
        glVertex2f(27.5, 20);
      }
    }
    glEnd();
  }
  glEnable(GL_DEPTH_TEST);
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
    draw_hud(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
