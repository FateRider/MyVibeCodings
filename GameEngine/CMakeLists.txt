cmake_minimum_required(VERSION 3.10)
project(MiniGameEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
file(GLOB SOURCES "src/*.cpp")

add_executable(GameEngine ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
#find_package(GLFW REQUIRED)


target_link_libraries(GameEngine PRIVATE OpenGL::GL GLEW::GLEW glfw)
