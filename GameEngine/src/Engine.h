#ifndef ENGINE_H
#define ENGINE_H

class Engine {
  public:
    Engine();
    ~Engine();

    void Run(); // Main loop

  private:
    bool Initialize();   // Init OpenGL, window etc.
    void ProcessInput(); // Handle keyboard etc.
    void Update();       // Physics and logic
    void Render();       // Draw objects, grid, etc.
    void Shutdown();     // Cleanup

    bool isRunning;
    struct GLFWwindow *window;
};

#endif
