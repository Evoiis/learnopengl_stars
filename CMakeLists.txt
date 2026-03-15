cmake_minimum_required(VERSION 3.10)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(main 
    src/main.cpp
    src/stb_image.cpp
)

target_compile_definitions(main PRIVATE 
    SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/"
)

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(main
    OpenGL::GL
    glfw
    GLEW::GLEW
)

