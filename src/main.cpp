#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// #include "stb_image.h"  // Used to load textures
#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MACos heh

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    // Vertices to describe a triangle
    // Note all values are within -1 to 1 range (normalized device coordinates)
    GLfloat vertices[] = {
        // Triangle 1       // vertex Colors
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // A
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // B
        0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    // C
        
        // Triangle 2
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // A
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // B
        0.0f, 0.0f, 0.7f,  1.0f, 1.0f, 1.0f,    // D

        // Triangle 3
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // A
        0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    // C
        0.0f, 0.0f, 0.7f,  1.0f, 1.0f, 1.0f,    // D

        // Triangle 4
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // B
        0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    // C
        0.0f, 0.0f, 0.7f,  1.0f, 1.0f, 1.0f,    // D
        
    };

    // Init Shader Program
    Shader shader_program(SHADER_DIR "vertex_shader.vs", SHADER_DIR "fragment_shader.fs");
    
    // Transformations
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    
    
    shader_program.use();
    unsigned int transformLoc = glGetUniformLocation(shader_program.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));


    // Init Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);    
    glBindVertexArray(VAO);

    // Generate Vertex Buffer Object
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy vertices data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 4th Parameter describes how we want th GPU to manage the given data
    // GL_STATIC_DRAW: the data will most likely not change at all or very rarely.
    // GL_DYNAMIC_DRAW: the data is likely to change a lot.
    // GL_STREAM_DRAW: the data will change every time it is drawn

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    /*
        glVertexAttribPointer parameters:
            - index
                - index of the generic vertex attribute to be modified
                    - matches location set in vertex_shader.vs
            - size
                - number of components for the vertex attribute
                    - 3 in this case because we provide x,y,z coordinates
            - type
                - data type of each component in the array
                    - in this case GL_FLOAT which matches our data in our vertices variable
            - normalized
                - whether the data should be normalized or not
                    - our data is already between -1 and 1 so we set this to false
            - stride
                - byte offset between consecutive vertex atrtibutes
                    - 6 * sizeof(float) because we have 3 components for position then 3 components for color for a total of 6
                    - When we only have position components we only needed 3 * sizeof(float)
            - pointer
                - offset to the first component of the first vertex attribute
                - 0 for the position components
                - below in the glVertexAttribPointer call for the color components you can see the offset is (3 * sizeof(float))
    */


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    
    // Wireframe Mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    transformLoc = glGetUniformLocation(shader_program.ID, "transform");

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // Clear depth buffer on each loop
        glClear(GL_DEPTH_BUFFER_BIT);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Transformation
        trans = glm::mat4(1.0f);
        // trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        float time = (float)glfwGetTime();
        trans = glm::translate(trans, glm::vec3(sin(time) * 0.5f, cos(time) * -0.5f, 0.0f));        
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        shader_program.use();

        // Transformation
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        
        // Triangle
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }  

    std::cout << "Terminating." << std::endl;
    glfwTerminate();
    return 0;
}


