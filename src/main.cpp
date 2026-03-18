#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>
#include <thread>

// #include "stb_image.h"  // Used to load textures
#include "shader.hpp"


struct Camera{
    glm::vec3 pos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up    = glm::vec3(0.0f, 1.0f,  0.0f);
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

}

void processCameraInput(GLFWwindow *window, Camera& cam, float delta_time){
    const float cameraSpeed = 2.5f * delta_time; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cam.pos += cameraSpeed * cam.front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cam.pos -= cameraSpeed * cam.front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        // std::cout << "A" << std::endl;
        cam.pos -= glm::normalize(glm::cross(cam.front, cam.up)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        // std::cout << "D" << std::endl;
        cam.pos += glm::normalize(glm::cross(cam.front, cam.up)) * cameraSpeed;
    }
}

float lastX = 400, lastY = 300;
float yaw = -90.0f;
float pitch = 0.f;
bool firstMouse = true;

void processMouseInput(GLFWwindow * window, double xpos, double ypos){
    // Ignore mouse input if button is not pressed
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS){
        firstMouse = true;
        return;
    }

    // Prevents pop in jerk on first mouse
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;
    if(pitch > 89.0f){
        pitch =  89.0f;
    }
    if(pitch < -89.0f){
        pitch = -89.0f;
    }
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MACos heh
    float width = 800;
    float height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
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

    // Camera
    const float radius = 10.0f;
    glm::mat4 view;
    Camera cam;
    glm::vec3 direction;
    
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cam.front = glm::normalize(direction);
    
    view = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);


    // Matrices

    // local -> world space
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // Making the view matrix in camera section
    // world -> view space
    // glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // Seems like we move the space around the camera, not move the camera around the space
    // ex: Star Trek 2009 space moves around Enterprise not the other way around, helps solve beaming to an object during warp
    
    // view space -> clip space
    glm::mat4 projection;
    // glm::perspective Parameters: FOV, Aspect Ratio (w/h), near plane, far plane
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    glm::mat4 mvp_composite = projection * view * model;
    int compositeLoc = glGetUniformLocation(shader_program.ID, "mvp_composite");
    glUniformMatrix4fv(compositeLoc, 1, GL_FALSE, glm::value_ptr(mvp_composite));


    // Wireframe Mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Depth On
    glEnable(GL_DEPTH_TEST);


    // Controls
    bool pause = false;
    bool space_was_pressed = false;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide mouse cursor
    glfwSetCursorPosCallback(window, processMouseInput);

    // Frame time
    float delta_time = 0.0f;
    float last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float now = (float)glfwGetTime();
        delta_time = now - last_frame_time;
        last_frame_time = now;


        bool space_pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if(space_pressed && !space_was_pressed){
            pause = !pause;
        }
        space_was_pressed = space_pressed;

        if(pause){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            glfwPollEvents();
            processInput(window);
            continue;
        }
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        
        // Clear depth buffer on each loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);
        processCameraInput(window, cam, delta_time);
        view = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cam.front = glm::normalize(direction);
        
        // Triangle Transformation
        trans = glm::mat4(1.0f);
        // trans = glm::translate(trans, glm::vec3(sin(now) * 0.5f, cos(now) * -0.5f, 0.0f));        
        // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        
        shader_program.use();
        
        // Move Camera aka Move triangle away 
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.1f));


        // Rotate camera around (0, 0, 0)
        // camX = sin(glfwGetTime()) * radius;
        // camZ = cos(glfwGetTime()) * radius;
        // view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));  
        
        // Update MVP Composite
        glm::mat4 mvp_composite = projection * view * model;
        glUniformMatrix4fv(compositeLoc, 1, GL_FALSE, glm::value_ptr(mvp_composite));
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


