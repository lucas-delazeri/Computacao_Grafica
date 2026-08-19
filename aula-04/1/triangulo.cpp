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
bool translating_left = false;
bool translating_right = false;
bool translating_bottom = false;
bool translating_top = false;

const GLfloat kDefaultTranslate = 0;
GLfloat translate_x = kDefaultTranslate;
GLfloat translate_y = kDefaultTranslate;
GLfloat translate_increment = 0.05f;

struct Rotating {
    bool bottom = false;
    bool left = false;
    bool top = false;
    bool right = false;
    bool rotating = false;
};

struct Scaling {
    bool scaling = false;
    bool x_increase = false;
    bool x_decrease = false;
    bool y_increase = false;
    bool y_decrease = false;
    bool z_increase = false;
    bool z_decrease = false;
};

Rotating rotating;
GLfloat rotate_angle_x = 0.0f;
GLfloat rotate_angle_y = 0.0f;
GLfloat rotate_angle_increment = 1.0f;

Scaling _scale;
const GLfloat kMaxScale = 5.0f;
const GLfloat kMinScale = 0.1f;
const GLfloat kDefaultScale = 1.0f;
GLfloat scale_increment = 0.01f;

GLfloat scale_x = kDefaultScale;
GLfloat scale_y = kDefaultScale;
GLfloat scale_z = kDefaultScale;

void keyboard_read(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        translate_x = kDefaultTranslate;
        translate_y = kDefaultTranslate;
        rotate_angle_x = 0.0f;
        rotate_angle_y = 0.0f;
        scale_x = kDefaultScale;
        scale_y = kDefaultScale;
        scale_z = kDefaultScale;
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
      rotating.rotating = true;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
      rotating.rotating = false;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      _scale.scaling = true;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
      _scale.scaling = false;
    }

    bool t_pressed = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS);

    translating_right = t_pressed && (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
    translating_left = t_pressed && (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
    translating_top = t_pressed && (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
    translating_bottom = t_pressed && (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);

    bool r_pressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);

    rotating.right = r_pressed && (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
    rotating.left = r_pressed && (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
    rotating.top = r_pressed && (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
    rotating.bottom = r_pressed && (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);

    bool e_pressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

    _scale.x_increase = e_pressed && (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
    _scale.x_decrease = e_pressed && (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
    _scale.y_increase = e_pressed && (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
    _scale.y_decrease = e_pressed && (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
    _scale.z_increase = e_pressed && (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
    _scale.z_decrease = e_pressed && (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);

}

void resize_window(GLFWwindow* window) {
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    if (window_height == 0) window_height = 1;

    glViewport(0, 0, window_width, window_height);
    GLdouble aspect_ratio = (GLdouble)window_width / (GLdouble)window_height;

    // Tipo de visão ed câmera (ortográfica ou perspectiva)
    glMatrixMode(GL_PROJECTION); // Aplica operações matriciais subsequentes à pilha de matrizes de projeção.
    glLoadIdentity(); // Substitui a matriz atual pela matriz identidade

    if (orthographic_view) {
        glfwSetWindowTitle(window, "Triangulo Ortogonal");
        GLdouble left = -kOrthographicLimitX;
        GLdouble right = kOrthographicLimitX;
        GLdouble bottom = -kOrthographicLimitY;
        GLdouble top = kOrthographicLimitY;

        if (aspect_ratio >= 1.0) {
            glOrtho(left * aspect_ratio, right * aspect_ratio, bottom, top, -kOrthographicLimitZ, kOrthographicLimitZ);
        } else {
            glOrtho(left, right, bottom / aspect_ratio, top / aspect_ratio, -kOrthographicLimitZ, kOrthographicLimitZ);
        }
    } else if (perspective_view) {
        glfwSetWindowTitle(window, "Triangulo em Perspectiva");
        gluPerspective(kPerspectiveFieldOfViewAngle, aspect_ratio, kPerspectiveNearZ, kPerspectiveFarZ);
    }
}

void update_state() {
    if (translating_right) {
      translate_x += translate_increment;
    }
    if (translating_left) {
      translate_x -= translate_increment;
    }
    if (translating_top) {
      translate_y += translate_increment;
    }
    if (translating_bottom) {
      translate_y -= translate_increment;
    }
    if (rotating.left) {
      rotate_angle_y -= rotate_angle_increment;
    }
    if (rotating.right) {
      rotate_angle_y += rotate_angle_increment;
    }
    if (rotating.top) {
      rotate_angle_x -= rotate_angle_increment;
    }
    if (rotating.bottom) {
      rotate_angle_x += rotate_angle_increment;
    }
    if (_scale.x_increase && scale_x < kMaxScale) {
      scale_x += scale_increment;
    }
    if (_scale.x_decrease && scale_x > kMinScale) scale_x -= scale_increment;
    if (_scale.y_increase && scale_y < kMaxScale) scale_y += scale_increment;
    if (_scale.y_decrease && scale_y > kMinScale) scale_y -= scale_increment;
    if (_scale.z_increase && scale_z < kMaxScale) scale_z += scale_increment;
    if (_scale.z_decrease && scale_z > kMinScale) scale_z -= scale_increment;
}

void draw() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (perspective_view) {
      glTranslatef(0.0f, 0.0f, -kPerspectiveTranslateZ);
    }
    glTranslatef(translate_x, translate_y, 0.0f);
    glRotatef(rotate_angle_x, 1.0f, 0.0f, 0.0f);
    glRotatef(rotate_angle_y, 0.0f, 1.0f, 0.0f);
    glScalef(scale_x, scale_y, scale_z);

    glBegin(GL_TRIANGLES);
    {
        glColor3ub(255, 0, 0);
        glVertex2f(0.0f, 2.5f);

        glColor3ub(0, 255, 0);
        glVertex2f(-2.5f, -2.5f);

        glColor3ub(0, 0, 255);
        glVertex2f(2.5f, -2.5f);
    }
    glEnd();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Demo", NULL, NULL);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao inicializar GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        keyboard_read(window);
        update_state();
        resize_window(window);
        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}