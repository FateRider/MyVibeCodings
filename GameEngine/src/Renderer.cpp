#include "Renderer.h"
#include <vector>

Renderer::Renderer() : axisVAO(0), axisVBO(0), gridVAO(0), gridVBO(0) {}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &axisVBO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
}

void Renderer::Init() {
    SetupAxes();
    SetupGrid(20, 1.0f); // 20x20 grid
}

void Renderer::SetupAxes() {
    float axisVertices[] = {
        // X axis - red
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        10.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        // Y axis - green
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        10.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        // Z axis - blue
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        10.0f,
        0.0f,
        0.0f,
        1.0f,
    };

    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::SetupGrid(int count, float spacing) {
    std::vector<float> vertices;
    float half = (count * spacing) / 2.0f;

    for (int i = 0; i <= count; ++i) {
        float offset = -half + i * spacing;

        // vertical line (Z)
        vertices.insert(vertices.end(), {offset, 0.0f, -half, 0.5f, 0.5f, 0.5f});
        vertices.insert(vertices.end(), {offset, 0.0f, half, 0.5f, 0.5f, 0.5f});

        // horizontal line (X)
        vertices.insert(vertices.end(), {-half, 0.0f, offset, 0.5f, 0.5f, 0.5f});
        vertices.insert(vertices.end(), {half, 0.0f, offset, 0.5f, 0.5f, 0.5f});
    }

    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::RenderAxes() {
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);
}

void Renderer::RenderGrid(int count, float spacing) {
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, count * 4 + 4);
}
