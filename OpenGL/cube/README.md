3D Rotating Cube with OpenGL and SDL2

This project renders a 3D rotating cube with colored vertices using OpenGL 3.3 core profile and SDL2 for window and context management. It uses GLSL shaders and GLM for transformations.

Features

- 3D cube rendered with vertex and fragment shaders
- Each vertex has a different color
- Rotation animation over time
- Perspective projection with camera transformation
- Depth testing enabled for proper 3D rendering

Requirements

- C++ compiler with C++11 or newer support
- SDL2 development libraries
- GLEW library for OpenGL function loading
- GLM library for matrix math
- OpenGL 3.3 capable GPU and drivers

Build Instructions (Linux example)

1. Install dependencies:

   - Debian/Ubuntu:
     sudo apt install build-essential libsdl2-dev libglew-dev libglm-dev

   - Fedora:
     sudo dnf install gcc-c++ SDL2-devel glew-devel glm-devel

   - Arch Linux:
     sudo pacman -S base-devel sdl2 glew glm

2. Compile the program:

   g++ main.cpp -o rotating_cube -lSDL2 -lGLEW -lGL

3. Run:

   ./rotating_cube

Code Structure

- main.cpp: Contains OpenGL initialization, shader compilation, vertex setup, and the main loop.
- Shaders: Vertex and fragment shaders are defined as inline GLSL strings.
- Vertex Data: Positions and colors combined in a single VBO.
- Transforms: Uses GLM to create model, view, and projection matrices.

Notes

- Ensure your graphics drivers support OpenGL 3.3 core profile.
- This code uses modern OpenGL with VAOs and VBOs.
- SDL2 handles window creation and event processing.
