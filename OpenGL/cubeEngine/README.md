# 3D Rotating Cube with Grid and Gizmo

This project demonstrates a simple 3D scene using OpenGL with SDL2 and GLM. It features a rotating colored cube, a grid on the XZ plane, and a gizmo showing the XYZ axes. The setup mimics a basic editor viewport like in game engines.

---

## Features

- **Rotating Cube**: A colorful cube rotating continuously.
- **Grid**: A gray grid on the XZ plane to help visualize the ground.
- **Gizmo**: Colored lines representing the X (red), Y (green), and Z (blue) axes.
- **Camera**: Fixed position with zoom controlled by mouse wheel.
- **Window Resize Handling**: Updates viewport and projection matrix on resize.
- **Single File**: Entire implementation in one `.cpp` file for simplicity.

---

## Controls

- **Mouse Wheel**: Zoom in/out by changing the camera's field of view (FOV).
- **Window Resize**: The rendering viewport and projection adjust automatically.
---

## Dependencies

- **SDL2**: Window creation and input handling.
- **GLEW**: OpenGL extension loader.
- **GLM**: Math library for matrix and vector operations.

Make sure these libraries are installed and properly linked when compiling.

---

## Compilation (Example)

```bash
g++ -std=c++11 -o cube main.cpp -lSDL2 -lGLEW -lGL
```

Adjust library flags as needed depending on your system.

---

## Code Overview

- Shader compilation and linking functions handle GLSL shader setup.
- Cube vertex data contains positions and colors for 36 vertices.
- Grid vertices are generated dynamically covering a 21x21 line grid on the XZ plane.
- Gizmo consists of 3 colored lines along the coordinate axes.
- The main loop processes SDL events, handles zoom input, and resizes viewport.
- Uses GLM to create model, view, and projection matrices.
- Draw calls for cube (triangles), grid (lines), and gizmo (lines).

---

## Possible Improvements

- Add keyboard/mouse control for camera movement (orbit, pan).
- Implement lighting for more realistic visuals.
- Allow toggling grid and gizmo visibility.
- Add object selection or manipulation tools.
- Use element buffer objects (EBO) for more efficient cube rendering.

---

This demo serves as a starting point for building a simple 3D editor or game engine viewport with basic rendering and interaction.

---

If you want me to add any features or explain parts of the code, just ask.
