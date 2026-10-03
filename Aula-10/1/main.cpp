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

#include "font.h"
#include "glb_model.h"

const GLfloat kCameraSpeed = 0.1;
const GLfloat kMouseSensitivity = 0.1;

const GLfloat kDefaultCameraEyePositionX = 0;
const GLfloat kDefaultCameraEyePositionY = 0;
const GLfloat kDefaultCameraEyePositionZ = -10;
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

const GLfloat kDefaultCameraRightX = -1;
const GLfloat kDefaultCameraRightY = 0;
const GLfloat kDefaultCameraRightZ = 0;
GLfloat camera_right_x = kDefaultCameraRightX;
GLfloat camera_right_y = kDefaultCameraRightY;
GLfloat camera_right_z = kDefaultCameraRightZ;

const GLfloat kDefaultCameraYaw = -90;
GLfloat camera_yaw = kDefaultCameraYaw;

const GLfloat kCameraPitchLimit = 90;
const GLfloat kDefaultCameraPitch = 0;
GLfloat camera_pitch = kDefaultCameraPitch;

const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1;
const GLfloat kPerspectiveFarZ = 100;
const GLfloat kPerspectiveTranslateZ = 13;

bool translating_right = false;
bool translating_left = false;
bool translating_foward = false;
bool translating_backward = false;
bool translating_up = false;
bool translating_down = false;

const GLfloat kTranslateLimit = 2;
const GLfloat kDefaultTranslate = 0;
const GLfloat kDefaultTranslateIncrement = 0.05;
GLfloat translate_x = kDefaultTranslate;
GLfloat translate_y = kDefaultTranslate;
GLfloat translate_z = kDefaultTranslate;
GLfloat translate_increment_x = kDefaultTranslateIncrement;
GLfloat translate_increment_y = kDefaultTranslateIncrement;
GLfloat translate_increment_z = kDefaultTranslateIncrement;

bool rotating_right = false;
bool rotating_left = false;
bool rotating_foward = false;
bool rotating_backward = false;
bool rotating_up = false;
bool rotating_down = false;

const GLfloat kRotateAngleLimit = 180;
const GLfloat kDefaultRotateAngle = 0;
const GLfloat kDefaultRotateIncrement = 1;
GLfloat rotate_x = kDefaultRotateAngle;
GLfloat rotate_y = kDefaultRotateAngle;
GLfloat rotate_z = kDefaultRotateAngle;
GLfloat rotate_increment_x = kDefaultRotateIncrement;
GLfloat rotate_increment_y = kDefaultRotateIncrement;
GLfloat rotate_increment_z = kDefaultRotateIncrement;

bool scaling_right = false;
bool scaling_left = false;
bool scaling_foward = false;
bool scaling_backward = false;
bool scaling_up = false;
bool scaling_down = false;

const GLfloat kScaleLimit = 5;
const GLfloat kScaleMinLimit = 0.1;
const GLfloat kDefaultScale = 1;
const GLfloat kDefaultScaleIncrement = 0.02;
GLfloat scale_x = kDefaultScale;
GLfloat scale_y = kDefaultScale;
GLfloat scale_z = kDefaultScale;
GLfloat scale_increment_x = kDefaultScaleIncrement;
GLfloat scale_increment_y = kDefaultScaleIncrement;
GLfloat scale_increment_z = kDefaultScaleIncrement;

GLuint red_ball_texture_id;
GLuint green_ball_texture_id;
GLuint blue_ball_texture_id;

GLuint model_display_list_id = 0;
GlbModel model;

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
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLenum texture_format = ((texture_channels == 4) ? GL_RGBA : GL_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, texture_format, texture_width, texture_height, 0, texture_format,
               GL_UNSIGNED_BYTE, texture_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(texture_data);
}

void create_model_display_list(GLuint& model_display_list_id, const GlbModel& model) {
  model_display_list_id = glGenLists(1);
  glNewList(model_display_list_id, GL_COMPILE);
  {
    glEnable(GL_TEXTURE_2D);
    draw_model(model);
    glDisable(GL_TEXTURE_2D);
  }
  glEndList();
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
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_eye_x -= camera_direction_x * kCameraSpeed;
    camera_eye_y -= camera_direction_y * kCameraSpeed;
    camera_eye_z -= camera_direction_z * kCameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera_eye_x += camera_right_x * kCameraSpeed;
    camera_eye_y += camera_right_y * kCameraSpeed;
    camera_eye_z += camera_right_z * kCameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera_eye_x -= camera_right_x * kCameraSpeed;
    camera_eye_y -= camera_right_y * kCameraSpeed;
    camera_eye_z -= camera_right_z * kCameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    camera_eye_x = kDefaultCameraEyePositionX;
    camera_eye_y = kDefaultCameraEyePositionY;
    camera_eye_z = kDefaultCameraEyePositionZ;
    camera_direction_x = kDefaultCameraDirectionX;
    camera_direction_y = kDefaultCameraDirectionY;
    camera_direction_z = kDefaultCameraDirectionZ;
    camera_right_x = kDefaultCameraRightX;
    camera_right_y = kDefaultCameraRightY;
    camera_right_z = kDefaultCameraRightZ;
    camera_yaw = kDefaultCameraYaw;
    camera_pitch = kDefaultCameraPitch;
    // Reinicia as transformações do modelo
    translate_x = translate_y = translate_z = kDefaultTranslate;
    translate_increment_x = translate_increment_y = translate_increment_z = kDefaultTranslateIncrement;

    rotate_x = rotate_y = rotate_z = kDefaultRotateAngle;
    rotate_increment_x = rotate_increment_y = rotate_increment_z = kDefaultRotateIncrement;

    scale_x = scale_y = scale_z = kDefaultScale;
    scale_increment_x = scale_increment_y = scale_increment_z = kDefaultScaleIncrement;

    hud_text << "Redefinido! ";
  }

  translating_up = false;
  translating_down = false;
  translating_right = false;
  translating_left = false;
  translating_foward = false;
  translating_backward = false;

  // TRANSFORMAÇÕES COM TECLADO
  if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    translating_up = true;
    hud_text << "Transladando para cima... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    translating_down = true;
    hud_text << "Transladando para baixo... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    translating_right = true;
    hud_text << "Transladando para direita... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    translating_left = true;
    hud_text << "Transladando para esquerda... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
    translating_foward = true;
    hud_text << "Transladando para frente... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
    translating_backward = true;
    hud_text << "Transladando para tras... ";
  }

  rotating_up = false;
  rotating_down = false;
  rotating_right = false;
  rotating_left = false;
  rotating_foward = false;
  rotating_backward = false;

  if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    rotating_up = true;
    hud_text << "Rotacionando para cima... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    rotating_down = true;
    hud_text << "Rotacionando para baixo... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    rotating_right = true;
    hud_text << "Rotacionando para direita... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    rotating_left = true;
    hud_text << "Rotacionando para esquerda... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
    rotating_foward = true;
    hud_text << "Rotacionando para frente... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
    rotating_backward = true;
    hud_text << "Rotacionando para tras... ";
  }

  scaling_up = false;
  scaling_down = false;
  scaling_right = false;
  scaling_left = false;
  scaling_foward = false;
  scaling_backward = false;

  if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    scaling_up = true;
    hud_text << "Escalonando para cima... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    scaling_down = true;
    hud_text << "Escalonando para baixo... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    scaling_right = true;
    hud_text << "Escalonando para direita... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    scaling_left = true;
    hud_text << "Escalonando para esquerda... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
    scaling_foward = true;
    hud_text << "Escalonando para frente... ";
  }

  if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
    scaling_backward = true;
    hud_text << "Escalonando para tras... ";
  }
}

void resize_window(GLFWwindow* window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
}

void draw(GLFWwindow* window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  GLdouble field_of_view = kPerspectiveFieldOfViewAngle;
  GLdouble near_z = kPerspectiveNearZ;
  GLdouble far_z = kPerspectiveFarZ;
  gluPerspective(field_of_view, aspect_ratio, near_z, far_z);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLfloat camera_center_x = (camera_eye_x + camera_direction_x);
  GLfloat camera_center_y = (camera_eye_y + camera_direction_y);
  GLfloat camera_center_z = (camera_eye_z + camera_direction_z);
  gluLookAt(camera_eye_x, camera_eye_y, camera_eye_z,           // eye
            camera_center_x, camera_center_y, camera_center_z,  // direction
            kCameraUpX, kCameraUpY, kCameraUpZ                  // up
  );

  if (translating_up) {
    translate_y += translate_increment_y;
    if ((translate_y < -kTranslateLimit) || (translate_y > kTranslateLimit)) {
      translate_increment_y *= -1;
    }
  }
  if (translating_down) {
    translate_y -= translate_increment_y;
    if ((translate_y < -kTranslateLimit) || (translate_y > kTranslateLimit)) {
      translate_increment_y *= -1;
    }
  }

  if (translating_right) {
    translate_x += translate_increment_x;
    if ((translate_x < -kTranslateLimit) || (translate_x > kTranslateLimit)) {
      translate_increment_x *= -1;
    }
  }
  if (translating_left) {
    translate_x -= translate_increment_x;
    if ((translate_x < -kTranslateLimit) || (translate_x > kTranslateLimit)) {
      translate_increment_x *= -1;
    }
  }

  if (translating_foward) {
    translate_z += translate_increment_z;
    if ((translate_z < -kTranslateLimit) || (translate_z > kTranslateLimit)) {
      translate_increment_z *= -1;
    }
  }
  if (translating_backward) {
    translate_z -= translate_increment_z;
    if ((translate_z < -kTranslateLimit) || (translate_z > kTranslateLimit)) {
      translate_increment_z *= -1;
    }
  }

  if (rotating_up) {
    rotate_x += rotate_increment_x;
    if ((rotate_x < -kRotateAngleLimit) || (rotate_x > kRotateAngleLimit)) {
      rotate_increment_x *= -1;
    }
  }
  if (rotating_down) {
    rotate_x -= rotate_increment_x;
    if ((rotate_x < -kRotateAngleLimit) || (rotate_x > kRotateAngleLimit)) {
      rotate_increment_x *= -1;
    }
  }

  if (rotating_right) {
    rotate_y += rotate_increment_y;
    if ((rotate_y < -kRotateAngleLimit) || (rotate_y > kRotateAngleLimit)) {
      rotate_increment_y *= -1;
    }
  }
  if (rotating_left) {
    rotate_y -= rotate_increment_y;
    if ((rotate_y < -kRotateAngleLimit) || (rotate_y > kRotateAngleLimit)) {
      rotate_increment_y *= -1;
    }
  }

  if (rotating_foward) {
    rotate_z += rotate_increment_z;
    if ((rotate_z < -kRotateAngleLimit) || (rotate_z > kRotateAngleLimit)) {
      rotate_increment_z *= -1;
    }
  }
  if (rotating_backward) {
    rotate_z -= rotate_increment_z;
    if ((rotate_z < -kRotateAngleLimit) || (rotate_z > kRotateAngleLimit)) {
      rotate_increment_z *= -1;
    }
  }

  if (scaling_up) {
    scale_y += scale_increment_y;
    if ((scale_y < kScaleMinLimit) || (scale_y > kScaleLimit)) {
      scale_increment_y *= -1;
    }
  }
  if (scaling_down) {
    scale_y -= scale_increment_y;
    if ((scale_y < kScaleMinLimit) || (scale_y > kScaleLimit)) {
      scale_increment_y *= -1;
    }
  }

  if (scaling_right) {
    scale_x += scale_increment_x;
    if ((scale_x < kScaleMinLimit) || (scale_x > kScaleLimit)) {
      scale_increment_x *= -1;
    }
  }
  if (scaling_left) {
    scale_x -= scale_increment_x;
    if ((scale_x < kScaleMinLimit) || (scale_x > kScaleLimit)) {
      scale_increment_x *= -1;
    }
  }

  if (scaling_foward) {
    scale_z += scale_increment_z;
    if ((scale_z < kScaleMinLimit) || (scale_z > kScaleLimit)) {
      scale_increment_z *= -1;
    }
  }
  if (scaling_backward) {
    scale_z -= scale_increment_z;
    if ((scale_z < kScaleMinLimit) || (scale_z > kScaleLimit)) {
      scale_increment_z *= -1;
    }
  }

  glTranslatef(translate_x, translate_y, translate_z - kPerspectiveTranslateZ);

  glRotatef(rotate_x, 1, 0, 0);
  glRotatef(rotate_y, 0, 1, 0);
  glRotatef(rotate_z, 0, 0, 1);

  glScalef(scale_x, scale_y, scale_z);

  // distancias uniformes entre as cadeiras
  for (int i = 0; i < 10; i++) {
    glPushMatrix();
    glTranslatef(i, i, i);
    glCallList(model_display_list_id);
    glPopMatrix();
  }
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

    if (translating_up || translating_down || translating_right || translating_left || translating_foward ||
        translating_backward) {
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

    if (rotating_up || rotating_down || rotating_right || rotating_left || rotating_foward || rotating_backward) {
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

    if (scaling_up || scaling_down || scaling_right || scaling_left || scaling_foward || scaling_backward) {
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
  glfwSetWindowTitle(window, "Modelo 3D com camera");
  glfwMakeContextCurrent(window);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_read);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  load_model(model, "models/beach_chair.glb");
  create_model_display_list(model_display_list_id, model);

  glEnable(GL_DEPTH_TEST);

  load_texture(red_ball_texture_id, "textures/red_ball.png");
  load_texture(green_ball_texture_id, "textures/green_ball.png");
  load_texture(blue_ball_texture_id, "textures/blue_ball.png");

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    resize_window(window);
    draw(window);
    draw_hud(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteLists(model_display_list_id, 1);
  destroy_model(model);
  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}