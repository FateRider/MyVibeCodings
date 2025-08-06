#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Renderer {
  public:
    Renderer();
    ~Renderer();

    void Init();
    void RenderAxes();
    void RenderGrid(int count = 20, float spacing = 1.0f);

  private:
    GLuint axisVAO, axisVBO;
    GLuint gridVAO, gridVBO;

    void SetupAxes();
    void SetupGrid(int count, float spacing);
};

#endif
