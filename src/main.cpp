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
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   
        0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        
        // Triangle 2
        0.9f, 0.9f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    // GLfloat and float are interchangeable
    // Second triangle data for multiple VBO/VAO
    // float tri2_vertices[] = {
    //     // Triangle 2
        // 0.9f, 0.9f, 0.0f,
        // 0.0f, 0.0f, 0.0f,
        // 0.5f, -0.5f, 0.0f
    // };

    // Rectangle
    // float vertices[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //     -0.5f,  0.5f, 0.0f   // top left 
    // };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // Texture
    // float texCoords[] = {
    //     0.0f, 0.0f,  // lower-left corner  
    //     1.0f, 0.0f,  // lower-right corner
    //     0.5f, 1.0f   // top-center corner
    // };

    // unsigned int texture;
    // glGenTextures(1, &texture);
    // glBindTexture(GL_TEXTURE_2D, texture);    
    
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    
    // int width, height, nrChannels;
    // unsigned char *data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
    // if (data)
    // {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else
    // {
    //     std::cout << "Failed to load texture" << std::endl;
    // }

    // // texture cleanup
    // stbi_image_free(data);

    // Init Shader Program
    Shader shader_program("./shaders/vertex_shader.vs","./shaders/fragment_shader.fs");


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
    


    // Triangle 2, with separate VAO & VBO
    // unsigned int VAO_2;
    // glGenVertexArrays(1, &VAO_2);
    // glBindVertexArray(VAO_2);

    // uint VBO_2;
    // glGenBuffers(1, &VBO_2);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(tri2_vertices), tri2_vertices, GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);



    // Rectangle
    // unsigned int EBO;
    // glGenBuffers(1, &EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    
    // Wireframe Mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glUseProgram(shaderProgram);
        shader_program.use();
        
        // Triangle
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Triangle 2
        // glBindVertexArray(VAO_2);
        // glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // Points
        // glDrawArrays(GL_POINTS, 0, 3);
        
        // Rectangle
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }  

    std::cout << "Terminating." << std::endl;
    glfwTerminate();
    return 0;
}


