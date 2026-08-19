#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>

// --- Configurações da Câmera / Projeção ---
bool orthographic_view = true;
const GLfloat kOrthographicLimitX = 5;
const GLfloat kOrthographicLimitY = 5;
const GLfloat kOrthographicLimitZ = 20;

bool perspective_view = false;
const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1;
const GLfloat kPerspectiveFarZ = 100;
const GLfloat kPerspectiveTranslateZ = -10.0f; // Distância padrão da câmera

// --- Transformações 3D ---
GLfloat translate_x = 0.0f, translate_y = 0.0f, translate_z = 0.0f;
GLfloat rotate_x = 0.0f, rotate_y = 0.0f, rotate_z = 0.0f;
GLfloat scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f;

// Velocidades de transformação (por segundo)
const GLfloat kTranslateSpeed = 4.0f;
const GLfloat kRotateSpeed = 90.0f; // graus por segundo
const GLfloat kScaleSpeed = 1.5f;

// --- Leitura do Teclado e Atualização da Lógica ---
void process_input(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Resetar transformações com ESPAÇO
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        translate_x = translate_y = translate_z = 0.0f;
        rotate_x = rotate_y = rotate_z = 0.0f;
        scale_x = scale_y = scale_z = 1.0f;
    }

    // Modos de Projeção
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        orthographic_view = true;
        perspective_view = false;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        orthographic_view = false;
        perspective_view = true;
    }

    // Checa modificadores segurados
    bool mode_t = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS);
    bool mode_r = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
    bool mode_e = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

    // --- Modo Translação (T) ---
    if (mode_t) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  translate_x -= kTranslateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) translate_x += kTranslateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)   translate_y -= kTranslateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)     translate_y += kTranslateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) translate_z -= kTranslateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)   translate_z += kTranslateSpeed * dt;
    }

    // --- Modo Rotação (R) ---
    if (mode_r) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  rotate_y -= kRotateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotate_y += kRotateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)   rotate_x += kRotateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)     rotate_x -= kRotateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) rotate_z -= kRotateSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)   rotate_z += kRotateSpeed * dt;
    }

    // --- Modo Escala (E) ---
    if (mode_e) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  scale_x = std::max(0.1f, scale_x - kScaleSpeed * dt);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) scale_x += kScaleSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)   scale_y = std::max(0.1f, scale_y - kScaleSpeed * dt);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)     scale_y += kScaleSpeed * dt;
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) scale_z = std::max(0.1f, scale_z - kScaleSpeed * dt);
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)   scale_z += kScaleSpeed * dt;
    }
}

void resize_window(GLFWwindow* window) {
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    if (window_height == 0) window_height = 1;

    glViewport(0, 0, window_width, window_height);
    GLdouble aspect_ratio = (GLdouble)window_width / window_height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (orthographic_view) {
        glfwSetWindowTitle(window, "Tetraedro Ortogonal");
        GLdouble left = -kOrthographicLimitX;
        GLdouble right = kOrthographicLimitX;
        GLdouble bottom = -kOrthographicLimitY;
        GLdouble top = kOrthographicLimitY;
        GLdouble near = -kOrthographicLimitZ;
        GLdouble far = kOrthographicLimitZ;

        if (window_width > window_height) {
            glOrtho(left * aspect_ratio, right * aspect_ratio, bottom, top, near, far);
        } else {
            glOrtho(left, right, bottom / aspect_ratio, top / aspect_ratio, near, far);
        }
    } else if (perspective_view) {
        glfwSetWindowTitle(window, "Tetraedro em Perspectiva");
        gluPerspective(kPerspectiveFieldOfViewAngle, aspect_ratio, kPerspectiveNearZ, kPerspectiveFarZ);
    }
}

void draw() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Se estiver em perspectiva, afasta a câmera antes de aplicar as transformações do objeto
    if (perspective_view) {
        glTranslatef(0.0f, 0.0f, kPerspectiveTranslateZ);
    }

    // 1. Translação do objeto
    glTranslatef(translate_x, translate_y, translate_z);

    // 2. Rotação do objeto
    glRotatef(rotate_x, 1.0f, 0.0f, 0.0f);
    glRotatef(rotate_y, 0.0f, 1.0f, 0.0f);
    glRotatef(rotate_z, 0.0f, 0.0f, 1.0f);

    // 3. Escala do objeto
    glScalef(scale_x, scale_y, scale_z);

    // Desenho da Geometria (Tetraedro)
    glBegin(GL_TRIANGLES);
    {
        // Face 1
        glColor3ub(255, 255, 255);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 0.0f,  1.0f, -1.0f);

        // Face 2
        glColor3ub(255, 0, 0);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 0.0f,  0.0f,  1.0f);

        // Face 3
        glColor3ub(0, 255, 0);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 0.0f,  1.0f, -1.0f);
        glVertex3f( 0.0f,  0.0f,  1.0f);

        // Face 4
        glColor3ub(0, 0, 255);
        glVertex3f( 0.0f,  1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f( 0.0f,  0.0f,  1.0f);
    }
    glEnd();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(800, 800, "Computação Gráfica", NULL, NULL);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao inicializar GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);

    double last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double current_time = glfwGetTime();
        float delta_time = static_cast<float>(current_time - last_time);
        last_time = current_time;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        process_input(window, delta_time);
        resize_window(window);
        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}