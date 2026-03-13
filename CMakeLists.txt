cmake_minimum_required(VERSION 3.10)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(main src/main.cpp)

target_link_libraries(main
    OpenGL::GL
    glfw
    GLEW::GLEW
)
