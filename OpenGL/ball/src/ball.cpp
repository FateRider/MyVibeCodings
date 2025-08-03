#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#define MAX_BALLS 3000

struct Ball {
    float x, y;
    float vx, vy;
    float radius;
    float r, g, b;
};

// Global top listesi
std::vector<Ball> balls;

// Rastgele float üretimi
float randFloat(float min, float max) { return min + ((float)rand() / (float)(RAND_MAX)) * (max - min); }

// Renk değiştir
void randomizeColor(Ball &b) {
    b.r = randFloat(0.2f, 1.0f);
    b.g = randFloat(0.2f, 1.0f);
    b.b = randFloat(0.2f, 1.0f);
}

// Çizim (kenarlıklı)
void drawBall(const Ball &b) {
    const int segments = 64;

    // Border
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(b.x, b.y);
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * M_PI / segments;
        glVertex2f(b.x + (b.radius + 0.005f) * cos(angle), b.y + (b.radius + 0.005f) * sin(angle));
    }
    glEnd();

    // Fill
    glColor3f(b.r, b.g, b.b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(b.x, b.y);
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * M_PI / segments;
        glVertex2f(b.x + b.radius * cos(angle), b.y + b.radius * sin(angle));
    }
    glEnd();
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    if (!glfwInit()) return -1;
    GLFWwindow *window = glfwCreateWindow(800, 600, "Ball Bounce", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // İlk top
    balls.push_back({0.0f, 0.0f, 0.01f, 0.007f, 0.05f, 1.0f, 0.0f, 0.0f});

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        std::vector<Ball> newBalls;

        for (Ball &b : balls) {
            b.x += b.vx;
            b.y += b.vy;

            bool bounced = false;

            if (b.x + b.radius > 1.0f) {
                b.x = 1.0f - b.radius;
                b.vx *= -1;
                bounced = true;
            } else if (b.x - b.radius < -1.0f) {
                b.x = -1.0f + b.radius;
                b.vx *= -1;
                bounced = true;
            }

            if (b.y + b.radius > 1.0f) {
                b.y = 1.0f - b.radius;
                b.vy *= -1;
                bounced = true;
            } else if (b.y - b.radius < -1.0f) {
                b.y = -1.0f + b.radius;
                b.vy *= -1;
                bounced = true;
            }

            // Çarpma olduysa:
            if (bounced) {
                randomizeColor(b); // renk değiştir
                fflush(stdout);

                // Sınırı geçmediyse ve limit dolmadıysa yeni top ekle
                if (balls.size() + newBalls.size() < MAX_BALLS) {
                    Ball newBall;
                    newBall.x = b.x;
                    newBall.y = b.y;
                    newBall.radius = b.radius;
                    newBall.vx = randFloat(-0.015f, 0.015f);
                    newBall.vy = randFloat(-0.015f, 0.015f);
                    randomizeColor(newBall);
                    newBalls.push_back(newBall);
                }
            }

            glLoadIdentity();
            drawBall(b);
        }

        balls.insert(balls.end(), newBalls.begin(), newBalls.end());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
