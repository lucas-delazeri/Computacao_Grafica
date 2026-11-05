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
#include <random>
#include <sstream>

#include "glb_model.h"

double last_time;

const GLfloat kCameraSpeed = 0.005;
const GLfloat kMouseSensitivity = 0.1;

const GLfloat kDefaultCameraEyePositionX = 0;
const GLfloat kDefaultCameraEyePositionY = 0.5;
const GLfloat kDefaultCameraEyePositionZ = -5.0;
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

const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1;
const GLfloat kPerspectiveFarZ = 100;
const GLfloat kPerspectiveTranslateZ = 10;

GLuint sand_texture_id;
GLuint water_texture_id;
GLuint brick_texture_id;

GLuint ball_model_display_list_id = 0;
GLuint beach_chair_model_display_list_id = 0;
GLuint sun_model_display_list_id = 0;
GLuint tree_model_display_list_id = 0;
GLuint diamond_model_display_list_id = 0;
GLuint buoy_model_display_list_id = 0;
GLuint firefly_model_display_list_id = 0;
GLuint campfire_model_display_list_id = 0;

GlbModel ball_model;
GlbModel beach_chair_model;
GlbModel sun_model;
GlbModel tree_model;
GlbModel diamond_model;
GlbModel buoy_model;
GlbModel firefly_model;
GlbModel campfire_model;

const GLfloat kSkyColorDay[] = {0.53, 0.81, 0.92, 1};

const GLfloat kSunAmbient[] = {0.01f, 0.01f, 0.008f, 1.0f};
const GLfloat kSunDiffuse[] = {0.08f, 0.08f, 0.04f, 1.0f};
const GLfloat kSunSpecular[] = {0.95, 0.97, 0.90, 1};
const GLfloat kFireflyEmission[] = {0.5, 0.5, 0.5, 0.5};
const GLfloat kCampfireEmission[] = {0.8f, 0.35f, 0.08f, 1.0f};
const GLfloat kSceneAmbient[] = {0.025f, 0.025f, 0.03f, 1.0f};

const GLfloat kSunConstantAttenuation = 0.1;
const GLfloat kSunLinearAttenuation = 0.05;
const GLfloat kSunQuadraticAttenuation = 0;

const GLfloat kSunScale = 0.2;
const GLfloat kSunTranslateIncrement = 0.01;
const GLfloat kDefaultSunPositionX = 0;
const GLfloat kDefaultSunPositionY = 2;
const GLfloat kDefaultSunPositionZ = 0;
const GLfloat kMinimumSunPositionY = 1;
GLfloat firefly_position[] = {kDefaultSunPositionX, kDefaultSunPositionY, kDefaultSunPositionZ, 1};
const int kFireflyCount = 12;
struct FireflyOffset {
  GLfloat x;
  GLfloat y;
  GLfloat z;
};
FireflyOffset firefly_offsets[kFireflyCount];

const GLfloat kWaterSpeedU = 0.03;
const GLfloat kWaterSpeedV = 0.07;
GLfloat water_u = 0;
GLfloat water_v = 0;

const GLfloat kBuoySpeed = 0.25f;
const GLfloat kBuoyTurnSpeed = 45.0f;
const GLfloat kBuoyMinX = -1.75f;
const GLfloat kBuoyMaxX = 1.75f;
const GLfloat kBuoyMinZ = 2.25f;
const GLfloat kBuoyMaxZ = 3.75f;
GLfloat buoy_position_x = 0.3f;
GLfloat buoy_position_z = 3.0f;
GLfloat buoy_heading = 35.0f;
GLfloat buoy_target_heading = buoy_heading;
bool buoy_turning_at_border = false;
double buoy_direction_change_timer = 0.0;
double buoy_next_direction_change = 2.0;
std::mt19937 buoy_random_engine(std::random_device{}());

const int kSplashParticleCount = 48;
const int kSplashParticlesPerImpact = 18;
const GLfloat kSplashGravity = 2.4f;
struct SplashParticle {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat velocity_x;
  GLfloat velocity_y;
  GLfloat velocity_z;
  GLfloat lifetime;
  GLfloat max_lifetime;
};
SplashParticle splash_particles[kSplashParticleCount] = {};
int next_splash_particle = 0;

const GLfloat kMaterialShininessDull = 16;
const GLfloat kMaterialShininessPlastic = 32;
const GLfloat kMaterialShininessPolished = 96;
const GLfloat kMaterialShininessMirror = 128;

const GLfloat kCampfirePositionX = -1.0f;
const GLfloat kCampfirePositionY = -0.01f;
const GLfloat kCampfirePositionZ = 1.0f;
const GLfloat kCampfireLightPosition[] = {
    kCampfirePositionX, 0.3f, kCampfirePositionZ, 1.0f};
const GLfloat kCampfireLightAmbient[] = {0.08f, 0.025f, 0.005f, 1.0f};
const GLfloat kCampfireLightDiffuse[] = {1.0f, 0.42f, 0.12f, 1.0f};
const GLfloat kCampfireLightSpecular[] = {0.8f, 0.35f, 0.08f, 1.0f};

const int kSmokeParticleCount = 80;
const GLfloat kSmokeEmissionRate = 20.0f;   // partículas por segundo
const GLfloat kSmokeOriginHeight = 0.15f;   // altura acima da fogueira onde nasce
const GLfloat kSmokeWindX = 0.03f;          // vento empurrando a fumaça
const GLfloat kSmokeMinSize = 6.0f;         // tamanho inicial (pixels)
const GLfloat kSmokeMaxSize = 28.0f;        // tamanho final (pixels)

struct SmokeParticle {
  GLfloat x, y, z;
  GLfloat velocity_x, velocity_y, velocity_z;
  GLfloat lifetime;
  GLfloat max_lifetime;
};
SmokeParticle smoke_particles[kSmokeParticleCount] = {};
int next_smoke_particle = 0;
double smoke_emission_accumulator = 0.0;

void initialize_firefly_offsets() {
  std::random_device random_device;
  std::mt19937 random_engine(random_device());
  std::uniform_real_distribution<GLfloat> random_coordinate(-1.0f, 1.0f);

    for (int i = 0; i < kFireflyCount; ++i) {
      GLfloat x;
      GLfloat y;
      GLfloat z;
      do {
        x = random_coordinate(random_engine);
        y = random_coordinate(random_engine);
        z = random_coordinate(random_engine);
      } while (x * x + y * y + z * z > 1.0f);

      firefly_offsets[i] = {x * 0.8f, y * 0.4f, z * 0.8f};
    }
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

void emit_splash() {
  std::uniform_real_distribution<GLfloat> random_horizontal_velocity(-0.45f, 0.45f);
  std::uniform_real_distribution<GLfloat> random_vertical_velocity(0.65f, 1.4f);
  std::uniform_real_distribution<GLfloat> random_lifetime(0.6f, 1.0f);

  for (int i = 0; i < kSplashParticlesPerImpact; ++i) {
    SplashParticle& particle = splash_particles[next_splash_particle];
    next_splash_particle = (next_splash_particle + 1) % kSplashParticleCount;

    particle.x = buoy_position_x;
    particle.y = 0.025f;
    particle.z = buoy_position_z;
    particle.velocity_x = random_horizontal_velocity(buoy_random_engine);
    particle.velocity_y = random_vertical_velocity(buoy_random_engine);
    particle.velocity_z = random_horizontal_velocity(buoy_random_engine);
    particle.lifetime = random_lifetime(buoy_random_engine);
    particle.max_lifetime = particle.lifetime;
  }
}

void update_splash_particles(double delta_time) {
  for (int i = 0; i < kSplashParticleCount; ++i) {
    SplashParticle& particle = splash_particles[i];
    if (particle.lifetime <= 0.0f) {
      continue;
    }

    particle.velocity_y -= kSplashGravity * static_cast<GLfloat>(delta_time);
    particle.x += particle.velocity_x * static_cast<GLfloat>(delta_time);
    particle.y += particle.velocity_y * static_cast<GLfloat>(delta_time);
    particle.z += particle.velocity_z * static_cast<GLfloat>(delta_time);
    particle.lifetime -= static_cast<GLfloat>(delta_time);
  }
}

void draw_splash_particles() {
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POINT_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_POINT_SMOOTH);
  glPointSize(4.0f);
  glDepthMask(GL_FALSE);
  glBegin(GL_POINTS);
  for (int i = 0; i < kSplashParticleCount; ++i) {
    const SplashParticle& particle = splash_particles[i];
    if (particle.lifetime <= 0.0f) {
      continue;
    }

    const GLfloat opacity = particle.lifetime / particle.max_lifetime;
    glColor4f(0.75f, 0.9f, 1.0f, opacity);
    glVertex3f(particle.x, particle.y, particle.z);
  }
  glEnd();
  glPopAttrib();
}

void emit_smoke_particle() {
  std::uniform_real_distribution<GLfloat> random_spread(-0.03f, 0.03f);
  std::uniform_real_distribution<GLfloat> random_velocity_xz(-0.02f, 0.02f);
  std::uniform_real_distribution<GLfloat> random_velocity_y(0.12f, 0.22f);
  std::uniform_real_distribution<GLfloat> random_lifetime(2.5f, 4.0f);

  SmokeParticle& particle = smoke_particles[next_smoke_particle];
  next_smoke_particle = (next_smoke_particle + 1) % kSmokeParticleCount;

  particle.x = kCampfirePositionX + random_spread(buoy_random_engine);
  particle.y = kCampfirePositionY + kSmokeOriginHeight;
  particle.z = kCampfirePositionZ + random_spread(buoy_random_engine);
  particle.velocity_x = random_velocity_xz(buoy_random_engine);
  particle.velocity_y = random_velocity_y(buoy_random_engine);
  particle.velocity_z = random_velocity_xz(buoy_random_engine);
  particle.lifetime = random_lifetime(buoy_random_engine);
  particle.max_lifetime = particle.lifetime;
}

void update_smoke_particles(double delta_time) {
  // Emite novas partículas de acordo com a taxa de emissão.
  smoke_emission_accumulator += kSmokeEmissionRate * delta_time;
  while (smoke_emission_accumulator >= 1.0) {
    emit_smoke_particle();
    smoke_emission_accumulator -= 1.0;
  }

  std::uniform_real_distribution<GLfloat> random_turbulence(-0.05f, 0.05f);
  const GLfloat dt = static_cast<GLfloat>(delta_time);

  for (int i = 0; i < kSmokeParticleCount; ++i) {
    SmokeParticle& particle = smoke_particles[i];
    if (particle.lifetime <= 0.0f) {
      continue;
    }

    // O vento e a turbulência desviam a fumaça para os lados.
    particle.velocity_x += (kSmokeWindX + random_turbulence(buoy_random_engine)) * dt;
    particle.velocity_z += random_turbulence(buoy_random_engine) * dt;

    particle.x += particle.velocity_x * dt;
    particle.y += particle.velocity_y * dt;
    particle.z += particle.velocity_z * dt;
    particle.lifetime -= dt;
  }
}

void draw_smoke_particles() {
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POINT_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_POINT_SMOOTH);
  glDepthMask(GL_FALSE);  // fumaça não escreve no depth buffer

  for (int i = 0; i < kSmokeParticleCount; ++i) {
    const SmokeParticle& particle = smoke_particles[i];
    if (particle.lifetime <= 0.0f) {
      continue;
    }

    // age vai de 0 (recém-nascida) até 1 (prestes a sumir)
    const GLfloat age = 1.0f - particle.lifetime / particle.max_lifetime;

    // Aparece rápido e desaparece devagar.
    const GLfloat fade_in = std::min(1.0f, age * 6.0f);
    const GLfloat opacity = 0.4f * fade_in * (1.0f - age);

    // Começa cinza escura e clareia com o tempo.
    const GLfloat gray = 0.25f + 0.35f * age;

    // Cresce conforme sobe.
    const GLfloat size = kSmokeMinSize + (kSmokeMaxSize - kSmokeMinSize) * age;

    glPointSize(size);  // não pode ser chamado dentro de glBegin/glEnd
    glBegin(GL_POINTS);
    glColor4f(gray, gray, gray, opacity);
    glVertex3f(particle.x, particle.y, particle.z);
    glEnd();
  }

  glPopAttrib();
}

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
    camera_yaw = kDefaultCameraYaw;
    camera_pitch = kDefaultCameraPitch;

    firefly_position[0] = kDefaultSunPositionX;
    firefly_position[1] = kDefaultSunPositionY;
    firefly_position[2] = kDefaultSunPositionZ;
  }

  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
      firefly_position[0] -= kSunTranslateIncrement;
    }
    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
      firefly_position[0] += kSunTranslateIncrement;
    }
    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
      if (firefly_position[1] > kMinimumSunPositionY) {
        firefly_position[1] -= kSunTranslateIncrement;
      }
    }
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
      firefly_position[1] += kSunTranslateIncrement;
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)) {
      firefly_position[2] -= kSunTranslateIncrement;
    }
    if ((glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)) {
      firefly_position[2] += kSunTranslateIncrement;
    }
  }
}

void resize_window(GLFWwindow* window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
}

void animate() {
  double current_time = glfwGetTime();
  double delta_time = current_time - last_time;
  last_time = current_time;

  water_u += kWaterSpeedU * delta_time;
  water_v += kWaterSpeedV * delta_time;

  const double buoy_delta_time = (delta_time > 0.1) ? 0.1 : delta_time;
  update_splash_particles(buoy_delta_time);
  update_smoke_particles(buoy_delta_time);

  buoy_direction_change_timer += buoy_delta_time;

  if (!buoy_turning_at_border && buoy_direction_change_timer >= buoy_next_direction_change) {
    std::uniform_real_distribution<GLfloat> random_turn(-1.0f, 1.0f);
    std::uniform_real_distribution<double> random_interval(2.0, 4.0);
    buoy_heading += 0.1;
    buoy_direction_change_timer = 0.0;
    buoy_next_direction_change = random_interval(buoy_random_engine);
  }

  if (buoy_turning_at_border) {
    GLfloat heading_difference = buoy_target_heading - buoy_heading;
    while (heading_difference > 180.0f) {
      heading_difference -= 360.0f;
    }
    while (heading_difference < -180.0f) {
      heading_difference += 360.0f;
    }

    const GLfloat max_turn = kBuoyTurnSpeed * buoy_delta_time;
    if (std::abs(heading_difference) <= max_turn) {
      buoy_heading = buoy_target_heading;
      buoy_turning_at_border = false;
    } else {
      buoy_heading += std::copysign(max_turn, heading_difference);
    }
  }

  const GLfloat radians = buoy_heading * 3.14159265f / 180.0f;
  buoy_position_x += std::sin(radians) * kBuoySpeed * buoy_delta_time;
  buoy_position_z += std::cos(radians) * kBuoySpeed * buoy_delta_time;

  bool hit_x_border = false;
  bool hit_z_border = false;
  if (buoy_position_x < kBuoyMinX || buoy_position_x > kBuoyMaxX) {
    buoy_position_x = (buoy_position_x < kBuoyMinX) ? kBuoyMinX : kBuoyMaxX;
    hit_x_border = true;
  }
  if (buoy_position_z < kBuoyMinZ || buoy_position_z > kBuoyMaxZ) {
    buoy_position_z = (buoy_position_z < kBuoyMinZ) ? kBuoyMinZ : kBuoyMaxZ;
    hit_z_border = true;
  }
  if ((hit_x_border || hit_z_border) && !buoy_turning_at_border) {
    emit_splash();

    buoy_target_heading = buoy_heading;
    if (hit_x_border) {
      buoy_target_heading = -buoy_target_heading;
    }
    if (hit_z_border) {
      buoy_target_heading = 180.0f - buoy_target_heading;
    }

    std::uniform_real_distribution<GLfloat> random_bounce(-20.0f, 20.0f);
    buoy_target_heading += random_bounce(buoy_random_engine);

    const GLfloat target_radians = buoy_target_heading * 3.14159265f / 180.0f;
    const bool heading_points_inward =
        (!hit_x_border ||
         ((buoy_position_x == kBuoyMinX) ? std::sin(target_radians) > 0.0f
                                         : std::sin(target_radians) < 0.0f)) &&
        (!hit_z_border ||
         ((buoy_position_z == kBuoyMinZ) ? std::cos(target_radians) > 0.0f
                                         : std::cos(target_radians) < 0.0f));
    if (!heading_points_inward) {
      buoy_target_heading = std::atan2(-buoy_position_x, 3.0f - buoy_position_z) *
                            180.0f / 3.14159265f;
    }

    buoy_turning_at_border = true;
  }
}

void draw(GLFWwindow* window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  GLdouble field_of_view = kPerspectiveFieldOfViewAngle;
  GLdouble near = kPerspectiveNearZ;
  GLdouble far = kPerspectiveFarZ;
  gluPerspective(field_of_view, aspect_ratio, near, far);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLfloat camera_center_x = (camera_eye_x + camera_direction_x);
  GLfloat camera_center_y = (camera_eye_y + camera_direction_y);
  GLfloat camera_center_z = (camera_eye_z + camera_direction_z);
  gluLookAt(camera_eye_x, camera_eye_y, camera_eye_z,           // eye
            camera_center_x, camera_center_y, camera_center_z,  // direction
            kCameraUpX, kCameraUpY, kCameraUpZ                  // up
  );

  glTranslatef(0, 0, -kPerspectiveTranslateZ);
  glLightfv(GL_LIGHT0, GL_POSITION, firefly_position);
  glLightfv(GL_LIGHT1, GL_POSITION, kCampfireLightPosition);

  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    {
      glTranslatef(water_u, water_v, 0.0f);
      glMatrixMode(GL_MODELVIEW);
      {
        const GLfloat kWaterSpecular[] = {0.12, 0.18, 0.5, 1};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kWaterSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kMaterialShininessMirror);

        glNormal3f(0, 1, 0);
        glColor3ub(255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, water_texture_id);
        {
          const GLfloat kWaterY = 0.01f;
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0);
          glVertex3f(-2, kWaterY, 2);
          glTexCoord2f(4, 0);
          glVertex3f(2, kWaterY, 2);
          glTexCoord2f(4, 4);
          glVertex3f(2, kWaterY, 4);
          glTexCoord2f(0, 4);
          glVertex3f(-2, kWaterY, 4);
          glEnd();
        }
        glDisable(GL_TEXTURE_2D);
      }
      glMatrixMode(GL_TEXTURE);
    }
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }
  glPopAttrib();
  glPopMatrix();

  draw_splash_particles();

  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    const GLfloat kPoolBorderY = 0.03f;
    const GLfloat kPoolBorderSpecular[] = {0.2, 0.18, 0.12, 1};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kPoolBorderSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kMaterialShininessDull);
    glNormal3f(0, 1, 0);
    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, brick_texture_id);
    glBegin(GL_QUADS);
    // Faixas frontal e traseira da borda.
    glTexCoord2f(0, 0);
    glVertex3f(-2.25f, kPoolBorderY, 1.75f);
    glTexCoord2f(9, 0);
    glVertex3f(2.25f, kPoolBorderY, 1.75f);
    glTexCoord2f(9, 0.5f);
    glVertex3f(2.25f, kPoolBorderY, 2);
    glTexCoord2f(0, 0.5f);
    glVertex3f(-2.25f, kPoolBorderY, 2);

    glTexCoord2f(0, 0);
    glVertex3f(-2.25f, kPoolBorderY, 4);
    glTexCoord2f(9, 0);
    glVertex3f(2.25f, kPoolBorderY, 4);
    glTexCoord2f(9, 0.5f);
    glVertex3f(2.25f, kPoolBorderY, 4.25f);
    glTexCoord2f(0, 0.5f);
    glVertex3f(-2.25f, kPoolBorderY, 4.25f);

    // Faixas laterais entre as extremidades das faixas frontal e traseira.
    glTexCoord2f(0, 0);
    glVertex3f(-2.25f, kPoolBorderY, 2);
    glTexCoord2f(0.5f, 0);
    glVertex3f(-2, kPoolBorderY, 2);
    glTexCoord2f(4.5f, 0.5f);
    glVertex3f(-2, kPoolBorderY, 4);
    glTexCoord2f(0, 0.5f);
    glVertex3f(-2.25f, kPoolBorderY, 4);

    glTexCoord2f(0, 0);
    glVertex3f(2, kPoolBorderY, 2);
    glTexCoord2f(0.5f, 0);
    glVertex3f(2.25f, kPoolBorderY, 2);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(2.25f, kPoolBorderY, 4);
    glTexCoord2f(0, 0.5f);
    glVertex3f(2, kPoolBorderY, 4);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }
  glPopAttrib();
  glPopMatrix();

  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    const GLfloat kSandSpecular[] = {0.2, 0.18, 0.12, 1};
    /* DIFERENÇA ENTRE glMaterialf  E glMaterialfv 
       glMaterialf = recebe um valor 
       glMaterialfv = recebe um vetor de valores RGBA para cores ou de direção
    */

    // parâmetros de material para o modelo de iluminação
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kSandSpecular); // reflexão specular da areia
    // parâmetro 'param' = valor para o qual o GL_SHININESS será definido, neste caso com kMaterialShininessDull.
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kMaterialShininessDull);

    glNormal3f(0, 1, 0);
    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sand_texture_id);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBegin(GL_QUADS);
    {
      glTexCoord2f(-5, -3);
      glVertex3f(-5, 0, -5);
      glTexCoord2f(5, -3);
      glVertex3f(5, 0, -5);
      glTexCoord2f(5, 7);
      glVertex3f(5, 0, 5);
      glTexCoord2f(-5, 7);
      glVertex3f(-5, 0, 5);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }
  glPopAttrib();
  glPopMatrix();

  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    const GLfloat kBallSpecular[] = {1, 1, 1, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kBallSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kMaterialShininessPlastic);

    glTranslatef(-0.5, -0.05, 0);
    glScalef(0.1, 0.1, 0.1);
    glRotatef(60, 0, 1, 0);

    glCallList(ball_model_display_list_id);
  }
  glPopAttrib();
  glPopMatrix();

  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    const GLfloat kChairSpecular[] = {0.4, 0.3, 0.2, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kChairSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kMaterialShininessDull);

    glRotatef(240, 0, 1, 0);
    glScalef(0.003f, 0.003f, 0.003f);
    glTranslatef(139.37f, 251.96f, 167.99f);

    glCallList(beach_chair_model_display_list_id);
  }
  glPopAttrib();
  glPopMatrix();

  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, kFireflyEmission);

    for (int i = 0; i < kFireflyCount; ++i) {
      glPushMatrix();

      glTranslatef(firefly_position[0] + firefly_offsets[i].x,
                   firefly_position[1] + firefly_offsets[i].y,
                   firefly_position[2] + firefly_offsets[i].z);
      glScalef(kSunScale, kSunScale, kSunScale);
      glCallList(firefly_model_display_list_id);

      glPopMatrix();
    }

  }
  glPopAttrib();
  glPopMatrix();

  // NOVO ELEMENTO
  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    const GLfloat kTreeSpecular[] = {0.4, 0.3, 0.2, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kTreeSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kMaterialShininessDull);

    glTranslatef(0, -0.1, 1);
    glScalef(5, 5, 5);

    glCallList(tree_model_display_list_id);
  }
  glPopAttrib();
  glPopMatrix();

  // NOVO ELEMENTO
  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    const GLfloat kDiamondSpecular[] = { 350 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kDiamondSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kMaterialShininessMirror);

    glTranslatef(2, 0, 1);
    glScalef(0.1, 0.1, 0.1);
    glRotatef(30, 1, 0, 0);

    glCallList(diamond_model_display_list_id);
  }
  glPopAttrib();
  glPopMatrix();

  // NOVO ELEMENTO
  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, kCampfireEmission);

    glTranslatef(kCampfirePositionX, kCampfirePositionY, kCampfirePositionZ);
    glScalef(0.1, 0.1, 0.1);

    glCallList(campfire_model_display_list_id);
  }
  glPopAttrib();
  glPopMatrix();

  // NOVO ELEMENTO
  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    const GLfloat kBuoySpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kBuoySpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kMaterialShininessMirror);
    glColor3ub(255, 0, 0);

    const GLfloat buoy_time = static_cast<GLfloat>(glfwGetTime());
    glTranslatef(buoy_position_x, -0.0001f + 0.015f * std::sin(buoy_time * 2.0f),
                 buoy_position_z);
    glRotatef(buoy_heading, 0, 1, 0);
    glRotatef(2.0f * std::sin(buoy_time * 1.5f), 0, 0, 1);
    glRotatef(1.5f * std::sin(buoy_time * 1.2f), 1, 0, 0);
    glScalef(0.1, 0.1, 0.1);

    glCallList(buoy_model_display_list_id);
  }
  glPopAttrib();
  glPopMatrix();

  draw_smoke_particles();
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  GLFWwindow* window = glfwCreateWindow(800, 600, "", NULL, NULL);
  if (!window) {
    std::cerr << "Falha ao criar a janela GLFW" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwSetWindowPos(window, 0, 0);
  glfwSetWindowTitle(window, "Cena 3D");
  glfwMakeContextCurrent(window);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_read);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  load_texture(sand_texture_id, "textures/grass.png");
  load_texture(water_texture_id, "textures/water.jpg");
  load_texture(brick_texture_id, "textures/brick.png");
  load_model(ball_model, "models/ball.glb");
  load_model(beach_chair_model, "models/beach_chair.glb");
  load_model(campfire_model, "models/campfire.glb");
  load_model(firefly_model, "models/firefly.glb");
  load_model(tree_model, "models/low-poly-coconut-palm-tree-3d-model-free/source/tree.glb");
  load_model(diamond_model, "models/blue_diamond.glb");
  load_model(buoy_model, "models/buoy.glb");

  initialize_firefly_offsets();

  create_model_display_list(ball_model_display_list_id, ball_model);
  create_model_display_list(beach_chair_model_display_list_id, beach_chair_model);
  create_model_display_list(campfire_model_display_list_id, campfire_model);
  create_model_display_list(firefly_model_display_list_id, firefly_model);
  create_model_display_list(tree_model_display_list_id, tree_model);
  create_model_display_list(diamond_model_display_list_id, diamond_model);
  create_model_display_list(buoy_model_display_list_id, buoy_model);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT0, GL_AMBIENT, kSunAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, kSunDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, kSunSpecular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, kSunConstantAttenuation);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, kSunLinearAttenuation);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, kSunQuadraticAttenuation);
  glLightfv(GL_LIGHT1, GL_AMBIENT, kCampfireLightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, kCampfireLightDiffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, kCampfireLightSpecular);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.35f);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.44f);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, kSceneAmbient);

  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(kSkyColorDay[0], kSkyColorDay[1], kSkyColorDay[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    resize_window(window);
    animate();
    draw(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteLists(ball_model_display_list_id, 1);
  glDeleteLists(beach_chair_model_display_list_id, 1);
  glDeleteLists(tree_model_display_list_id, 1);
  glDeleteLists(diamond_model_display_list_id, 1);
  glDeleteLists(buoy_model_display_list_id, 1);
  glDeleteLists(firefly_model_display_list_id, 1);
  glDeleteLists(campfire_model_display_list_id, 1);

  destroy_model(firefly_model);
  destroy_model(campfire_model);
  destroy_model(tree_model);
  destroy_model(ball_model);
  destroy_model(beach_chair_model);
  destroy_model(diamond_model);
  destroy_model(buoy_model);
  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
