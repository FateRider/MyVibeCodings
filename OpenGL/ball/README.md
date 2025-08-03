# Ball Bounce Simulation

## Description  
This project is a simple 2D ball bounce simulation using OpenGL (with GLEW and GLFW). Balls bounce inside a square viewport (-1 to 1 in both x and y axes). Each ball has its own position, velocity, radius, and color.

### Features:  
- Balls move and bounce off the window borders.  
- On every bounce, the ball changes its color randomly.  
- When a ball bounces, a new ball is created at the collision point with a random velocity and color.  
- Maximum number of balls is limited to 3000.  
- Each ball is drawn with a colored fill and a thin black border.

## Requirements  
- OpenGL development libraries  
- GLEW  
- GLFW  
- C++ compiler (supporting C++11 or later)

## How to Build  
Assuming you have `g++` and the required libraries installed:

```bash
g++ -o ball_bounce main.cpp -lGL -lGLEW -lglfw
```

(Replace `main.cpp` with your source filename.)

## How to Run  
```bash
./ball_bounce
```

A window will open displaying the bouncing balls.

## Code Overview  
- `Ball` struct holds position (`x`, `y`), velocity (`vx`, `vy`), radius, and color (`r`, `g`, `b`).  
- `randFloat(min, max)` generates random floats within a range.  
- `randomizeColor(Ball&)` changes the color of a ball randomly.  
- `drawBall(const Ball&)` draws a filled circle with a black border using OpenGL immediate mode.  
- Main loop updates each ball’s position, checks for collisions, changes color on bounce, and spawns new balls.

---

Created by Rider
