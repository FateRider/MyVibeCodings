#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Vertex shader (3D, renkli)
const char *vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

// Fragment shader
const char *fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main(){
    FragColor = vec4(vertexColor, 1.0);
}
)";

GLuint CompileShader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error:\n" << infoLog << "\n";
    }
    return shader;
}

GLuint CreateShaderProgram(const char *vertexSource, const char *fragmentSource) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader link error:\n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL init error: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow("3D Rotating Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "Window creation error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Küp vertexleri (pozisyon ve renk) 36 vertex (6 yüz * 2 üçgen * 3 vertex)
    float vertices[] = {
        // Positions          // Colors
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // kırmızı
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // yeşil
        0.5f,  0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, // mavi
        0.5f,  0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 1.0f, // magenta
        0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, // cyan
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // white
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 1.0f,

        -0.5f, 0.5f,  0.5f,  1.0f, 0.5f, 0.0f,                                        // turuncu
        -0.5f, 0.5f,  -0.5f, 0.0f, 0.5f, 1.0f,                                        // mavi
        -0.5f, -0.5f, -0.5f, 0.5f, 1.0f, 0.5f,                                        // yeşil
        -0.5f, -0.5f, -0.5f, 0.5f, 1.0f, 0.5f, -0.5f, -0.5f, 0.5f,  0.3f, 0.3f, 0.3f, // gri
        -0.5f, 0.5f,  0.5f,  1.0f, 0.5f, 0.0f,

        0.5f,  0.5f,  0.5f,  0.6f, 0.4f, 0.2f, 0.5f,  0.5f,  -0.5f, 0.2f, 0.6f, 0.4f, 0.5f,  -0.5f, -0.5f, 0.4f, 0.2f, 0.6f, 0.5f, -0.5f, -0.5f, 0.4f, 0.2f, 0.6f, 0.5f,  -0.5f, 0.5f, 0.7f, 0.7f, 0.2f, 0.5f,  0.5f,  0.5f,  0.6f, 0.4f, 0.2f,

        -0.5f, -0.5f, -0.5f, 0.9f, 0.1f, 0.1f, 0.5f,  -0.5f, -0.5f, 0.1f, 0.9f, 0.1f, 0.5f,  -0.5f, 0.5f,  0.1f, 0.1f, 0.9f, 0.5f, -0.5f, 0.5f,  0.1f, 0.1f, 0.9f, -0.5f, -0.5f, 0.5f, 0.9f, 0.9f, 0.1f, -0.5f, -0.5f, -0.5f, 0.9f, 0.1f, 0.1f,

        -0.5f, 0.5f,  -0.5f, 0.3f, 0.6f, 0.9f, 0.5f,  0.5f,  -0.5f, 0.9f, 0.3f, 0.6f, 0.5f,  0.5f,  0.5f,  0.6f, 0.9f, 0.3f, 0.5f, 0.5f,  0.5f,  0.6f, 0.9f, 0.3f, -0.5f, 0.5f,  0.5f, 0.3f, 0.6f, 0.9f, -0.5f, 0.5f,  -0.5f, 0.3f, 0.6f, 0.9f,
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Pozisyon
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Renk
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST); // Derinlik testi açık, 3D için gerekli

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float timeValue = SDL_GetTicks() / 1000.0f;

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), timeValue * glm::radians(50.0f), glm::vec3(1.0f, 0.3f, 0.5f));
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
