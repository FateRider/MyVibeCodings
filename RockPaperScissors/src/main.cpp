// rps_modern.cpp
// Modern OpenGL Rock-Paper-Scissors simulation with improved physics and behavior
// Requirements: glad, glfw, stb_image
// Compile example (Linux):
// g++ rps_modern.cpp -o rps_modern -lglfw -ldl -lGL -pthread

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.hpp"

#include "../include/glad/glad.hpp"
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

enum ObjectType { ROCK = 0, PAPER = 1, SCISSORS = 2 };

struct GameObject {
    ObjectType type;
    float x, y;
    float vx, vy;
    GLuint tex;
    bool alive;
};

const int WIDTH = 900;
const int HEIGHT = 700;
const int COUNT_PER_TYPE = 20;
std::vector<GameObject> objects;

static void checkShaderCompile(GLuint id, const std::string &name) {
    GLint ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, ' ');
        glGetShaderInfoLog(id, len, nullptr, &log[0]);
        std::cerr << "Shader compile error (" << name << "):\n" << log << "\n";
    }
}

static void checkProgramLink(GLuint prog) {
    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, ' ');
        glGetProgramInfoLog(prog, len, nullptr, &log[0]);
        std::cerr << "Program link error:\n" << log << "\n";
    }
}

GLuint LoadTexture(const char *path) {
    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &w, &h, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << "\n";
        return 0u;
    }
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

const char *vertexSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

uniform vec2 uOffset;
uniform vec2 uScale;
out vec2 vTex;

void main() {
    vec2 pos = aPos * uScale + uOffset;
    gl_Position = vec4(pos, 0.0, 1.0);
    vTex = aTex;
}
)";

const char *fragmentSrc = R"(
#version 330 core
in vec2 vTex;
out vec4 FragColor;
uniform sampler2D uTex;

void main() {
    vec4 tex = texture(uTex, vTex);
    if (tex.a < 0.01) discard;
    FragColor = tex;
}
)";

GLuint compileShaderProgram() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, nullptr);
    glCompileShader(vs);
    checkShaderCompile(vs, "vertex");

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, nullptr);
    glCompileShader(fs);
    checkShaderCompile(fs, "fragment");

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    checkProgramLink(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

void createQuad(GLuint &VAO, GLuint &VBO) {
    float quad[] = {-0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  1.0f, 1.0f,

                    0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f};
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void CreateObject(ObjectType type, GLuint tex) {
    GameObject o;
    o.type = type;

    float margin = 0.8f;
    float spacing = 0.3f;
    int baseX = rand() % 5;
    int baseY = rand() % 5;
    float jitter = 0.05f;

    o.x = -margin + baseX * spacing + ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f) * jitter;
    o.y = -margin + baseY * spacing + ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f) * jitter;

    auto rndV = []() -> float { return ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f) * 0.2f; };
    o.vx = rndV();
    o.vy = rndV();

    o.tex = tex;
    o.alive = true;

    objects.push_back(o);
}

int CountAlive(ObjectType t) {
    int c = 0;
    for (auto &o : objects)
        if (o.alive && o.type == t) ++c;
    return c;
}

void UpdatePositions(float dt) {
    for (auto &o : objects) {
        if (!o.alive) continue;
        o.x += o.vx * dt;
        o.y += o.vy * dt;

        const float margin = 0.09f;
        if (o.x > 1.0f - margin) {
            o.x = 1.0f - margin;
            o.vx = -o.vx;
        }
        if (o.x < -1.0f + margin) {
            o.x = -1.0f + margin;
            o.vx = -o.vx;
        }
        if (o.y > 1.0f - margin) {
            o.y = 1.0f - margin;
            o.vy = -o.vy;
        }
        if (o.y < -1.0f + margin) {
            o.y = -1.0f + margin;
            o.vy = -o.vy;
        }
    }
}

void UpdateCollisions() {
    const float collideDist = 0.12f;
    const float collideDistSq = collideDist * collideDist;
    const float separationFactor = 1.5f;

    for (size_t i = 0; i < objects.size(); ++i) {
        if (!objects[i].alive) continue;
        for (size_t j = i + 1; j < objects.size(); ++j) {
            if (!objects[j].alive) continue;

            float dx = objects[i].x - objects[j].x;
            float dy = objects[i].y - objects[j].y;
            float distSq = dx * dx + dy * dy;

            if (distSq < collideDistSq) {
                float dist = sqrt(distSq);
                if (dist < 0.001f) continue; // skip if too close

                float overlap = collideDist - dist;
                float nx = dx / dist;
                float ny = dy / dist;

                // strong push apart
                objects[i].x += nx * (overlap / 2.0f) * separationFactor;
                objects[i].y += ny * (overlap / 2.0f) * separationFactor;
                objects[j].x -= nx * (overlap / 2.0f) * separationFactor;
                objects[j].y -= ny * (overlap / 2.0f) * separationFactor;

                // calculate normal components of velocity
                float vi_dot_n = objects[i].vx * nx + objects[i].vy * ny;
                float vj_dot_n = objects[j].vx * nx + objects[j].vy * ny;

                float vi_t_x = objects[i].vx - vi_dot_n * nx;
                float vi_t_y = objects[i].vy - vi_dot_n * ny;
                float vj_t_x = objects[j].vx - vj_dot_n * nx;
                float vj_t_y = objects[j].vy - vj_dot_n * ny;

                // swap normal components
                float vi_n_x = vj_dot_n * nx;
                float vi_n_y = vj_dot_n * ny;
                float vj_n_x = vi_dot_n * nx;
                float vj_n_y = vi_dot_n * ny;

                objects[i].vx = vi_t_x + vi_n_x;
                objects[i].vy = vi_t_y + vi_n_y;
                objects[j].vx = vj_t_x + vj_n_x;
                objects[j].vy = vj_t_y + vj_n_y;

                // clamp speeds to max/min limits
                auto clampSpeed = [](float &vx, float &vy, float maxSpeed = 0.5f, float minSpeed = 0.05f) {
                    float speed = sqrt(vx * vx + vy * vy);
                    if (speed > maxSpeed) {
                        vx = (vx / speed) * maxSpeed;
                        vy = (vy / speed) * maxSpeed;
                    } else if (speed < minSpeed) {
                        float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 6.2831853f;
                        vx = cos(angle) * minSpeed;
                        vy = sin(angle) * minSpeed;
                    }
                };
                clampSpeed(objects[i].vx, objects[i].vy);
                clampSpeed(objects[j].vx, objects[j].vy);

                // rock-paper-scissors logic: type conversion
                ObjectType A = objects[i].type;
                ObjectType B = objects[j].type;
                if (A == B) {
                    std::swap(objects[i].vx, objects[j].vx);
                    std::swap(objects[i].vy, objects[j].vy);
                } else {
                    bool iWins = false;
                    if (A == ROCK && B == SCISSORS) iWins = true;
                    if (A == SCISSORS && B == PAPER) iWins = true;
                    if (A == PAPER && B == ROCK) iWins = true;

                    if (iWins) {
                        objects[j].type = A;
                        objects[j].tex = objects[i].tex;
                    } else {
                        objects[i].type = B;
                        objects[i].tex = objects[j].tex;
                    }
                }
            }
        }
    }
}

// Process input: closes window when ESC is pressed
void ProcessInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

int main() {
    std::srand((unsigned)std::time(nullptr));

    if (!glfwInit()) {
        std::cerr << "glfwInit failed\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "RPS Modern OpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    GLuint prog = compileShaderProgram();
    GLuint VAO, VBO;
    createQuad(VAO, VBO);

    GLuint rockTex = LoadTexture("./images/rock.png");
    GLuint paperTex = LoadTexture("./images/paper.png");
    GLuint scissorTex = LoadTexture("./images/scissors.png");
    if (!rockTex || !paperTex || !scissorTex) { std::cerr << "Warning: texture(s) failed to load.\n"; }

    for (int i = 0; i < COUNT_PER_TYPE; i++)
        CreateObject(ROCK, rockTex);
    for (int i = 0; i < COUNT_PER_TYPE; i++)
        CreateObject(PAPER, paperTex);
    for (int i = 0; i < COUNT_PER_TYPE; i++)
        CreateObject(SCISSORS, scissorTex);

    glUseProgram(prog);
    GLint locOffset = glGetUniformLocation(prog, "uOffset");
    GLint locScale = glGetUniformLocation(prog, "uScale");
    GLint locTex = glGetUniformLocation(prog, "uTex");
    glUniform1i(locTex, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double lastTime = glfwGetTime();

    bool winnerShown = false;
    std::string winnerText;

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        double now = glfwGetTime();
        float dt = float(now - lastTime);
        if (dt > 0.05f) dt = 0.05f;
        lastTime = now;

        if (!winnerShown) {
            UpdatePositions(dt);
            UpdateCollisions();
        }

        int rocks = CountAlive(ROCK);
        int papers = CountAlive(PAPER);
        int scissors = CountAlive(SCISSORS);

        bool gameOver = false;

        if ((rocks > 0 && papers == 0 && scissors == 0)) {
            winnerText = "ROCK";
            gameOver = true;
        } else if ((papers > 0 && rocks == 0 && scissors == 0)) {
            winnerText = "PAPER";
            gameOver = true;
        } else if ((scissors > 0 && rocks == 0 && papers == 0)) {
            winnerText = "SCISSORS";
            gameOver = true;
        }

        if (gameOver && !winnerShown) {
            winnerShown = true;
            std::cout << "Winner: " << winnerText << "\n";
        }

        if (winnerShown) {
            // Green background to highlight winner state
            glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        } else {
            glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        glUseProgram(prog);
        glBindVertexArray(VAO);

        for (auto &o : objects) {
            if (!o.alive) continue;
            float scaleX = 0.12f;
            float scaleY = 0.12f * (float(HEIGHT) / float(WIDTH));
            glUniform2f(locOffset, o.x, o.y);
            glUniform2f(locScale, scaleX, scaleY);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, o.tex ? o.tex : 0u);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // When winnerShown is true, just wait for user to press ESC to close window
    }

    glfwTerminate();
    return 0;
}
