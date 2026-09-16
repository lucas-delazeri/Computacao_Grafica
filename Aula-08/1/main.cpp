// Copyright
// Computação Gráfica
// URI Santiago
// Professor Laurence

#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "libs/font.h"
#include "libs/stb_image.h"

const GLfloat kCameraSpeed = 0.01;
const GLfloat kMouseSensitivity = 0.1;

const GLfloat kDefaultCameraEyePositionX = 0;
const GLfloat kDefaultCameraEyePositionY = 0;
const GLfloat kDefaultCameraEyePositionZ = 0;
GLfloat camera_eye_x = kDefaultCameraEyePositionX;
GLfloat camera_eye_y = kDefaultCameraEyePositionY;
GLfloat camera_eye_z = kDefaultCameraEyePositionZ;

const GLfloat kDefaultCameraDirectionX = 0;
const GLfloat kDefaultCameraDirectionY = 0;
const GLfloat kDefaultCameraDirectionZ = -1;
GLfloat camera_direction_x = kDefaultCameraDirectionX;
GLfloat camera_direction_y = kDefaultCameraDirectionY;
GLfloat camera_direction_z = kDefaultCameraDirectionZ;

const GLfloat kCameraUpX = 0;
const GLfloat kCameraUpY = 1;
const GLfloat kCameraUpZ = 0;

GLfloat camera_right_x;
GLfloat camera_right_y;
GLfloat camera_right_z;

const GLfloat kDefaultCameraYaw = -90;
GLfloat camera_yaw = kDefaultCameraYaw;

const GLfloat kCameraPitchLimit = 90;
const GLfloat kDefaultCameraPitch = 0;
GLfloat camera_pitch = kDefaultCameraPitch;

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
const GLfloat kRotateAngleIncrement = 0.1;
GLfloat rotate_angle_x = kDefaultRotateAngle;
GLfloat rotate_angle_y = kDefaultRotateAngle;
GLfloat rotate_angle_z = kDefaultRotateAngle;

bool scaling = false;
const GLfloat kScaleLimit = 2;
const GLfloat kDefaultScale = 1;
const GLfloat kScaleIncrement = 0.01;
GLfloat scale_x = kDefaultScale;
GLfloat scale_y = kDefaultScale;
GLfloat scale_z = kDefaultScale;

GLuint red_ball_texture_id;
GLuint green_ball_texture_id;
GLuint blue_ball_texture_id;
GLuint brick_texture_id;

const GLfloat kHudHeight = 300;
const GLfloat kHudWidth = 300;
std::ostringstream hud_text;

void load_texture(GLuint& texture_id, const std::string& filepath) {
  int texture_width, texture_height, texture_channels;
  uint8_t* texture_data =
      stbi_load(filepath.c_str(), &texture_width, &texture_height, &texture_channels, 0);

  if (!texture_data) {
    std::cerr << "Erro ao carregar a textura: " << filepath << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  GLenum texture_format = ((texture_channels == 4) ? GL_RGBA : GL_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, texture_format, texture_width, texture_height, 0, texture_format,
               GL_UNSIGNED_BYTE, texture_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(texture_data);
}

static void mouse_read(GLFWwindow* window, double mouse_x, double mouse_y) {
  static bool mouse_initialized = false;
  static GLfloat last_mouse_x = 0, last_mouse_y = 0;
  if (!mouse_initialized) {
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;
    mouse_initialized = true;
  }

  GLfloat mouse_delta_x = (mouse_x - last_mouse_x);
  GLfloat mouse_delta_y = (last_mouse_y - mouse_y);
  last_mouse_x = mouse_x;
  last_mouse_y = mouse_y;

  camera_pitch += (mouse_delta_y * kMouseSensitivity);
  if (camera_pitch > kCameraPitchLimit) {
    camera_pitch = kCameraPitchLimit;
  }
  if (camera_pitch < -kCameraPitchLimit) {
    camera_pitch = -kCameraPitchLimit;
  }
  camera_yaw += (mouse_delta_x * kMouseSensitivity);

  GLfloat camera_pitch_radians = camera_pitch * (3.1415 / 180);
  GLfloat camera_yaw_radians = camera_yaw * (3.1415 / 180);
  camera_direction_x = cosf(camera_yaw_radians) * cosf(camera_pitch_radians);
  camera_direction_y = sinf(camera_pitch_radians);
  camera_direction_z = sinf(camera_yaw_radians) * cosf(camera_pitch_radians);

  camera_right_x = camera_direction_z * kCameraUpY - camera_direction_y * kCameraUpZ;
  camera_right_y = camera_direction_x * kCameraUpZ - camera_direction_z * kCameraUpX;
  camera_right_z = camera_direction_y * kCameraUpX - camera_direction_x * kCameraUpY;
}

void keyboard_read(GLFWwindow* window) {
  hud_text.str("");

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_eye_x += camera_direction_x * kCameraSpeed;
    camera_eye_y += camera_direction_y * kCameraSpeed;
    camera_eye_z += camera_direction_z * kCameraSpeed;

    hud_text << "Movendo Camera para frente! ";
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_eye_x -= camera_direction_x * kCameraSpeed;
    camera_eye_y -= camera_direction_y * kCameraSpeed;
    camera_eye_z -= camera_direction_z * kCameraSpeed;

    hud_text << "Movendo Camera para tras! ";
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera_eye_x += camera_right_x * kCameraSpeed;
    camera_eye_y += camera_right_y * kCameraSpeed;
    camera_eye_z += camera_right_z * kCameraSpeed;

    hud_text << "Movendo Camera para esquerda! ";
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera_eye_x -= camera_right_x * kCameraSpeed;
    camera_eye_y -= camera_right_y * kCameraSpeed;
    camera_eye_z -= camera_right_z * kCameraSpeed;

    hud_text << "Movendo Camera para direita! ";
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    camera_eye_y -= kCameraSpeed;

    hud_text << "Movendo Camera para baixo! ";
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    camera_eye_y += kCameraSpeed;

    hud_text << "Movendo Camera para cima! ";
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    camera_eye_x = kDefaultCameraEyePositionX;
    camera_eye_y = kDefaultCameraEyePositionY;
    camera_eye_z = kDefaultCameraEyePositionZ;
    camera_direction_x = kDefaultCameraDirectionX;
    camera_direction_y = kDefaultCameraDirectionY;
    camera_direction_z = kDefaultCameraDirectionZ;
    camera_yaw = kDefaultCameraYaw;
    camera_pitch = kDefaultCameraPitch;

    translate_x = kDefaultTranslate;
    translate_y = kDefaultTranslate;
    translate_z = kDefaultTranslate;

    rotate_angle_x = kDefaultRotateAngle;
    rotate_angle_y = kDefaultRotateAngle;
    rotate_angle_z = kDefaultRotateAngle;

    scale_x = kDefaultScale;
    scale_y = kDefaultScale;
    scale_z = kDefaultScale;

    hud_text << "Redefinido! ";
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
    rotating = false;
    scaling = false;

    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
      if ((translate_x > -kTranslateLimit)) {
        translate_x -= kTranslateIncrement;
      }
      hud_text << "Transladando para esquerda";
    }
    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
      if ((translate_x < kTranslateLimit)) {
        translate_x += kTranslateIncrement;
      }
      hud_text << "Transladando para direita";
    }
    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
      if ((translate_y > -kTranslateLimit)) {
        translate_y -= kTranslateIncrement;
      }
      hud_text << "Transladando para baixo";
    }
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
      if ((translate_y < kTranslateLimit)) {
        translate_y += kTranslateIncrement;
      }
      hud_text << "Transladando para cima";
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)) {
      if ((translate_z > -kTranslateLimit)) {
        translate_z -= kTranslateIncrement;
      }
      hud_text << "Transladando para tras";
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)) {
      if ((translate_z < kTranslateLimit)) {
        translate_z += kTranslateIncrement;
      }
      hud_text << "Transladando para frente";
    }
  }

  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    translating = false;
    rotating = true;
    scaling = false;

    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
      if ((rotate_angle_x < kRotateAngleLimit)) {
        rotate_angle_x += kRotateAngleIncrement;
      }
      hud_text << "Rotacionando para baixo";
    }
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
      if ((rotate_angle_x > -kRotateAngleLimit)) {
        rotate_angle_x -= kRotateAngleIncrement;
      }
      hud_text << "Rotacionando para cima";
    }
    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
      if ((rotate_angle_y > -kRotateAngleLimit)) {
        rotate_angle_y -= kRotateAngleIncrement;
      }
      hud_text << "Rotacionando para esquerda";
    }
    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
      if ((rotate_angle_y < kRotateAngleLimit)) {
        rotate_angle_y += kRotateAngleIncrement;
      }
      hud_text << "Rotacionando para direita";
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)) {
      if ((rotate_angle_z > -kRotateAngleLimit)) {
        rotate_angle_z -= kRotateAngleIncrement;
      }
      hud_text << "Rotacionando para esquerda Eixo Z";
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)) {
      if ((rotate_angle_z < kRotateAngleLimit)) {
        rotate_angle_z += kRotateAngleIncrement;
      }
      hud_text << "Rotacionando para direita Eixo Z";
    }
  }

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    translating = false;
    rotating = false;
    scaling = true;

    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
      if ((scale_x > 0.1f)) {
        scale_x -= kScaleIncrement;
      }
      hud_text << "Escalando para esquerda";
    }
    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
      if ((scale_x < kScaleLimit)) {
        scale_x += kScaleIncrement; 
      }
      hud_text << "Escalando para direita";
    }
    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
      if ((scale_y > 0.1f)) {
        scale_y -= kScaleIncrement;
      }
      hud_text << "Escalando para baixo";
    }
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
      if ((scale_y < kScaleLimit)) {
        scale_y += kScaleIncrement;
      }
      hud_text << "Escalando para cima";
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)) {
      if ((scale_z > 0.1f)) {
        scale_z -= kScaleIncrement;
      }
      hud_text << "Escalando para tras";
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)) {
      if ((scale_z < kScaleLimit)) {
        scale_z += kScaleIncrement;
      }
      hud_text << "Escalando para frente";
    }
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

void draw(GLFWwindow* window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (orthographic_view) {
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
  } else if (perspective_view) {
    GLdouble field_of_view = kPerspectiveFieldOfViewAngle;
    GLdouble near = kPerspectiveNearZ;
    GLdouble far = kPerspectiveFarZ;
    gluPerspective(field_of_view, aspect_ratio, near, far);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLfloat camera_center_x = (camera_eye_x + camera_direction_x);
  GLfloat camera_center_y = (camera_eye_y + camera_direction_y);
  GLfloat camera_center_z = (camera_eye_z + camera_direction_z);
  gluLookAt(camera_eye_x, camera_eye_y, camera_eye_z,           // eye
            camera_center_x, camera_center_y, camera_center_z,  // direction
            kCameraUpX, kCameraUpY, kCameraUpZ                  // up
  );

  glTranslatef(translate_x, translate_y, (translate_z - kPerspectiveTranslateZ));

  glRotatef(rotate_angle_x, 1, 0, 0);
  glRotatef(rotate_angle_y, 0, 1, 0);
  glRotatef(rotate_angle_z, 0, 0, 1);

  glScalef(scale_x, scale_y, scale_z);

  glColor3ub(255, 255, 255);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, brick_texture_id);
 glBegin(GL_QUADS);

  {    // Frente
    glTexCoord2f(0, 0);
    glVertex3f(-2, -2, 2);
    glTexCoord2f(1, 0);
    glVertex3f(2, -2, 2);
    glTexCoord2f(1, 1);
    glVertex3f(2, 2, 2);
    glTexCoord2f(0, 1);
    glVertex3f(-2, 2, 2);

    // Trás
    glTexCoord2f(0, 0);
    glVertex3f(-2, -2, -2);
    glTexCoord2f(0, 1);
    glVertex3f(-2, 2, -2);
    glTexCoord2f(1, 1);
    glVertex3f(2, 2, -2);
    glTexCoord2f(1, 0);
    glVertex3f(2, -2, -2);

    // Esquerda
    glTexCoord2f(0, 0);
    glVertex3f(-2, -2, -2);
    glTexCoord2f(1, 0);
    glVertex3f(-2, -2, 2);
    glTexCoord2f(1, 1);
    glVertex3f(-2, 2, 2);
    glTexCoord2f(0, 1);
    glVertex3f(-2, 2, -2);

    // Direita
    glTexCoord2f(0, 0);
    glVertex3f(2, -2, -2);
    glTexCoord2f(0, 1);
    glVertex3f(2, 2, -2);
    glTexCoord2f(1, 1);
    glVertex3f(2, 2, 2);
    glTexCoord2f(1, 0);
    glVertex3f(2, -2, 2);

    // Cima
    glTexCoord2f(0, 0);
    glVertex3f(-2, 2, -2);
    glTexCoord2f(0, 1);
    glVertex3f(-2, 2, 2);
    glTexCoord2f(1, 1);
    glVertex3f(2, 2, 2);
    glTexCoord2f(1, 0);
    glVertex3f(2, 2, -2);

    // Baixo
    glTexCoord2f(0, 0);
    glVertex3f(-2, -2, -2);
    glTexCoord2f(1, 0);
    glVertex3f(2, -2, -2);
    glTexCoord2f(1, 1);
    glVertex3f(2, -2, 2);
    glTexCoord2f(0, 1);
    glVertex3f(-2, -2, 2);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);
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

    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);

    if (translating) {
      glBindTexture(GL_TEXTURE_2D, red_ball_texture_id);
      glBegin(GL_QUADS);
      {
        glTexCoord2f(0, 0);
        glVertex2f(5, 15);
        glTexCoord2f(1, 0);
        glVertex2f(15, 15);
        glTexCoord2f(1, 1);
        glVertex2f(15, 25);
        glTexCoord2f(0, 1);
        glVertex2f(5, 25);
      }
      glEnd();
    }

    if (rotating) {
      glBindTexture(GL_TEXTURE_2D, green_ball_texture_id);
      glBegin(GL_QUADS);
      {
        glTexCoord2f(0, 0);
        glVertex2f(15, 15);
        glTexCoord2f(1, 0);
        glVertex2f(25, 15);
        glTexCoord2f(1, 1);
        glVertex2f(25, 25);
        glTexCoord2f(0, 1);
        glVertex2f(15, 25);
      }
      glEnd();
    }

    if (scaling) {
      glBindTexture(GL_TEXTURE_2D, blue_ball_texture_id);
      glBegin(GL_QUADS);
      {
        glTexCoord2f(0, 0);
        glVertex2f(25, 15);
        glTexCoord2f(1, 0);
        glVertex2f(35, 15);
        glTexCoord2f(1, 1);
        glVertex2f(35, 25);
        glTexCoord2f(0, 1);
        glVertex2f(25, 25);
      }
      glEnd();
    }

    glDisable(GL_TEXTURE_2D);
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
  glfwSetWindowTitle(window, "Cubo com textura e camera");
  glfwMakeContextCurrent(window);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_read);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  glEnable(GL_DEPTH_TEST);

  load_texture(red_ball_texture_id, "textures/red_ball.png");
  load_texture(green_ball_texture_id, "textures/green_ball.png");
  load_texture(blue_ball_texture_id, "textures/blue_ball.png");
  load_texture(brick_texture_id, "textures/brick.png");

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    resize_window(window);
    draw(window);
    draw_hud(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
