#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// Vertex shader source code
// This shader transforms vertex positions by model, view, and projection matrices
// and passes vertex color to the fragment shader.
const char *vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;    // Vertex position input
layout(location = 1) in vec3 aColor;  // Vertex color input

out vec3 vertexColor;                 // Output color to fragment shader

uniform mat4 model;                   // Model transformation matrix
uniform mat4 view;                    // View (camera) matrix
uniform mat4 projection;              // Projection matrix

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0); // Transform vertex
    vertexColor = aColor;                                     // Pass color forward
}
)";

// Fragment shader source code
// This shader simply outputs the interpolated color from the vertex shader.
const char *fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;   // Input color from vertex shader
out vec4 FragColor;    // Final output color

void main(){
    FragColor = vec4(vertexColor, 1.0);  // Set fragment color with full opacity
}
)";

// Compile a shader of given type (vertex or fragment) from source code
GLuint CompileShader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);        // Create shader object
    glShaderSource(shader, 1, &source, nullptr); // Set shader source code
    glCompileShader(shader);                     // Compile shader

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // Check compile status
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log); // Retrieve error log
        std::cerr << "Shader compile error:\n" << log << "\n";
    }
    return shader;
}

// Create shader program by compiling vertex and fragment shaders and linking them
GLuint CreateShaderProgram(const char *vertexSrc, const char *fragSrc) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

    GLuint program = glCreateProgram();      // Create shader program
    glAttachShader(program, vertexShader);   // Attach vertex shader
    glAttachShader(program, fragmentShader); // Attach fragment shader
    glLinkProgram(program);                  // Link the program

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success); // Check link status
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log); // Retrieve link errors
        std::cerr << "Shader link error:\n" << log << "\n";
    }

    // Shaders can be deleted after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Cube vertex data: 36 vertices (6 faces * 2 triangles * 3 vertices each)
// Each vertex has 3 position floats and 3 color floats
float cubeVertices[] = {
    // positions          // colors (RGB)
    -0.5f, -0.5f, -0.5f, 1,    0,    0, // red face
    0.5f,  -0.5f, -0.5f, 0,    1,    0,    0.5f,  0.5f,  -0.5f, 0,    0,    1,    0.5f,  0.5f,  -0.5f, 0,    0,    1,    -0.5f, 0.5f,  -0.5f, 1,    1,    0,    -0.5f, -0.5f, -0.5f, 1,    0,    0,

    -0.5f, -0.5f, 0.5f,  1,    0,    1, // magenta face
    0.5f,  -0.5f, 0.5f,  0,    1,    1,    0.5f,  0.5f,  0.5f,  1,    1,    1,    0.5f,  0.5f,  0.5f,  1,    1,    1,    -0.5f, 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  1,    0,    1,

    -0.5f, 0.5f,  0.5f,  1,    0.5f, 0, // orange face
    -0.5f, 0.5f,  -0.5f, 0,    0.5f, 1,    -0.5f, -0.5f, -0.5f, 0.5f, 1,    0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 1,    0.5f, -0.5f, -0.5f, 0.5f,  0.3f, 0.3f, 0.3f, -0.5f, 0.5f,  0.5f,  1,    0.5f, 0,

    0.5f,  0.5f,  0.5f,  0.6f, 0.4f, 0.2f, // brown face
    0.5f,  0.5f,  -0.5f, 0.2f, 0.6f, 0.4f, 0.5f,  -0.5f, -0.5f, 0.4f, 0.2f, 0.6f, 0.5f,  -0.5f, -0.5f, 0.4f, 0.2f, 0.6f, 0.5f,  -0.5f, 0.5f,  0.7f, 0.7f, 0.2f, 0.5f,  0.5f,  0.5f,  0.6f, 0.4f, 0.2f,

    -0.5f, -0.5f, -0.5f, 0.9f, 0.1f, 0.1f, // red shade
    0.5f,  -0.5f, -0.5f, 0.1f, 0.9f, 0.1f, 0.5f,  0.5f,  -0.5f, 0.1f, 0.1f, 0.9f, 0.5f,  0.5f,  -0.5f, 0.1f, 0.1f, 0.9f, -0.5f, 0.5f,  -0.5f, 0.9f, 0.9f, 0.1f, -0.5f, -0.5f, -0.5f, 0.9f, 0.1f, 0.1f};

// Grid vertices container
// We will generate vertices dynamically for a grid on XZ plane
std::vector<float> gridVertices;

// Generate a grid on the XZ plane from -10 to 10 with 21 lines
// Each line consists of two points (start and end) with color grey (0.7f,0.7f,0.7f)
void generateGrid() {
    gridVertices.clear();
    int lines = 21;
    float start = -10.0f;
    float end = 10.0f;
    float y = 0.0f; // grid lies on Y=0 plane
    for (int i = 0; i < lines; ++i) {
        float pos = start + i * ((end - start) / (lines - 1));
        // Line parallel to Z axis
        gridVertices.push_back(pos);
        gridVertices.push_back(y);
        gridVertices.push_back(start);
        gridVertices.push_back(0.7f);
        gridVertices.push_back(0.7f);
        gridVertices.push_back(0.7f);

        gridVertices.push_back(pos);
        gridVertices.push_back(y);
        gridVertices.push_back(end);
        gridVertices.push_back(0.7f);
        gridVertices.push_back(0.7f);
        gridVertices.push_back(0.7f);

        // Line parallel to X axis
        gridVertices.push_back(start);
        gridVertices.push_back(y);
        gridVertices.push_back(pos);
        gridVertices.push_back(0.7f);
        gridVertices.push_back(0.7f);
        gridVertices.push_back(0.7f);

        gridVertices.push_back(end);
        gridVertices.push_back(y);
        gridVertices.push_back(pos);
        gridVertices.push_back(0.7f);
        gridVertices.push_back(0.7f);
        gridVertices.push_back(0.7f);
    }
}

// Gizmo vertices: lines representing XYZ axes colored Red, Green, Blue respectively
float gizmoVertices[] = {
    // X axis line (red)
    0,
    0,
    0,
    1,
    0,
    0,
    1,
    0,
    0,
    1,
    0,
    0,

    // Y axis line (green)
    0,
    0,
    0,
    0,
    1,
    0,
    0,
    1,
    0,
    0,
    1,
    0,

    // Z axis line (blue)
    0,
    0,
    0,
    0,
    0,
    1,
    0,
    0,
    1,
    0,
    0,
    1,
};

int main(int argc, char **argv) {
    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL init error: " << SDL_GetError() << "\n";
        return -1;
    }

    // Set OpenGL version to 3.3 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create resizable SDL window with OpenGL context
    SDL_Window *window = SDL_CreateWindow("3D Cube with Grid and Gizmo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize GLEW to load OpenGL extensions
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Compile and link shaders into a shader program
    GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Setup cube VAO and VBO
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Generate grid vertices and setup VAO/VBO for grid
    generateGrid();
    GLuint gridVAO, gridVBO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Setup gizmo VAO/VBO for axes lines
    GLuint gizmoVAO, gizmoVBO;
    glGenVertexArrays(1, &gizmoVAO);
    glGenBuffers(1, &gizmoVBO);
    glBindVertexArray(gizmoVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoVertices), gizmoVertices, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);

    bool running = true;
    SDL_Event event;

    // Initial window size
    int width = 800, height = 600;
    float fov = 45.0f; // Field of view angle in degrees (for zoom)

    while (running) {
        // Poll and handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            // Handle window resize event
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    width = event.window.data1;
                    height = event.window.data2;
                    glViewport(0, 0, width, height);
                }
            }

            // Handle mouse wheel zoom in/out
            else if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y > 0 && fov > 10.0f) fov -= 2.0f; // zoom in
                else if (event.wheel.y < 0 && fov < 90.0f)
                    fov += 2.0f; // zoom out
            }
        }

        // Clear screen with dark gray color and clear depth buffer
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate time in seconds for animation
        float timeSec = SDL_GetTicks() / 1000.0f;

        // Calculate model matrix: rotating cube over time around arbitrary axis
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), timeSec * glm::radians(45.0f), glm::vec3(1.0f, 0.3f, 0.5f));

        // View matrix: camera positioned 5 units back along Z axis
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

        // Projection matrix: perspective projection with current FOV and aspect ratio
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);

        // Use shader program for rendering
        glUseProgram(shaderProgram);

        // Pass transformation matrices to shader uniforms
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw rotating cube (triangles)
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw grid (lines) with identity model matrix (no transform)
        glm::mat4 identity = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, (GLsizei)(gridVertices.size() / 6));

        // Draw gizmo axes (lines)
        glBindVertexArray(gizmoVAO);
        glDrawArrays(GL_LINES, 0, 6);

        // Swap buffers to display the frame
        SDL_GL_SwapWindow(window);
    }

    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &gizmoVAO);
    glDeleteBuffers(1, &gizmoVBO);
    glDeleteProgram(shaderProgram);

    // Cleanup SDL resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
