Rotating Triangle with OpenGL and SDL2

This program renders a single rotating triangle using OpenGL 3.3 core profile and SDL2 for window and context management. It uses GLSL shaders and GLM for matrix transformations.

Features

- Renders a triangle with a solid color
- Smooth rotation animation around the Z-axis
- Uses modern OpenGL with shaders, VAO, and VBO
- SDL2 handles window creation and input events

Requirements

- C++ compiler with C++11 or newer support
- SDL2 development libraries
- GLEW library for OpenGL function loading
- GLM library for matrix math
- OpenGL 3.3 capable GPU and drivers

Build Instructions

1. Install dependencies:

   - Debian/Ubuntu:
     sudo apt install build-essential libsdl2-dev libglew-dev libglm-dev

   - Fedora:
     sudo dnf install gcc-c++ SDL2-devel glew-devel glm-devel

   - Arch Linux:
     sudo pacman -S base-devel sdl2 glew glm

2. Compile the program:

   g++ main.cpp -o rotating_triangle -lSDL2 -lGLEW -lGL

3. Run:

   ./rotating_triangle

Code Overview

- main.cpp contains OpenGL initialization, shader compilation, vertex setup, and main loop
- Vertex shader applies a transformation matrix for rotation
- Fragment shader colors the triangle with a fixed color
- GLM creates the rotation matrix updated each frame

Notes

- Ensure your system supports OpenGL 3.3 core profile
- Uses SDL2 event loop for window and input handling
