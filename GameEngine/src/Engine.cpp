#include "Engine.h"
#include "Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Global renderer instance
Renderer renderer;

Engine::Engine() : isRunning(false), window(nullptr) {}

Engine::~Engine() { Shutdown(); }

bool Engine::Initialize() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return false;
    }

    window = glfwCreateWindow(800, 600, "Mini Game Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        return false;
    }

    glEnable(GL_DEPTH_TEST);

    // Renderer init (setup VAOs for axes & grid)
    renderer.Init();

    return true;
}

void Engine::Run() {
    if (!Initialize()) return;

    isRunning = true;

    while (!glfwWindowShouldClose(window)) {
        ProcessInput();
        Update();
        Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Shutdown();
}

void Engine::ProcessInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void Engine::Update() {
    // Placeholder for physics or logic update
}

void Engine::Render() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw coordinate axes and grid
    renderer.RenderAxes();
    renderer.RenderGrid();
}

void Engine::Shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
