#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
    // pointScale ; // Can scale pointscale if we need here
}  

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

}

void processCameraInput(GLFWwindow *window, Camera& cam, float delta_time){
    float cameraSpeed = 4.f * delta_time;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        cameraSpeed *= 4.f;
    }
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
float pointScale;

void processMouseInput(GLFWwindow * window, double xpos, double ypos){
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS){
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

struct StarVertex {
    glm::vec3 position;
    float magnitude;
    float color;
};

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // TODO macos check

    float width = 1280;
    float height = 720;

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

    // IMGUI Init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Init Shader Program
    Shader shader_program(SHADER_DIR "point_sprites.vs", SHADER_DIR "point_sprites.fs");
    shader_program.use();

    // Init Stars
    std::vector<StarVertex> stars = {
        {{ 0.0f,  1.0f,  -5.0f}, -1.46f, 0.5f}, // Sirius-like,  blue-white
        {{ 1.5f,  0.5f,  -8.0f},  0.45f, 0.71f}, // Vega-like,    white
        {{ 0.5f,  0.5f,  -6.f},   2.5f, 3.5f}, // sun-like,     yellow-white
        {{ 3.0f, -0.5f, -20.0f},  2.80f, 3.50f}, // K-type,       orange
        {{-2.5f,  1.5f, -30.0f},  4.10f, 4.20f}, // M-type,       red
        // mid-range fill
        {{ 2.0f,  3.0f, -15.0f},  2.10f, 0.50f},
        {{-3.0f, -1.0f, -18.0f},  3.20f, 2.10f},
        {{ 0.5f, -2.0f, -25.0f},  3.80f, 1.40f},
        {{-1.5f,  0.5f, -10.0f},  1.90f, 0.90f},
        {{ 4.0f,  1.0f, -35.0f},  4.50f, 3.80f},
        // dim background
        {{ 1.0f,  4.0f, -40.0f},  5.50f, 1.20f},
        {{-4.0f,  2.0f, -50.0f},  -2.f, 4.0f},
        {{ 2.5f, -3.0f, -45.0f},  5.80f, 2.90f},
        {{-0.5f, -4.0f, -60.0f},  6.20f, 1.80f},
        {{ 5.0f,  0.5f, -55.0f},  5.90f, 3.20f},
    };


    // Init Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);    
    glBindVertexArray(VAO);

    // Generate Vertex Buffer Object
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy vertices data into buffer's memory
    glBufferData(GL_ARRAY_BUFFER, stars.size() * sizeof(StarVertex), stars.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StarVertex), (void*)offsetof(StarVertex, position));
    glEnableVertexAttribArray(0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(StarVertex), (void*)offsetof(StarVertex, magnitude));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(StarVertex), (void*)offsetof(StarVertex, color));

    // Init Point Scale - controls star size
    pointScale = 2.0f;
    glUniform1f(glGetUniformLocation(shader_program.ID, "uPointScale"), height * pointScale);


    // Camera (view matrix)
    const float radius = 10.0f;
    glm::mat4 view;
    Camera cam;
    glm::vec3 direction;
    
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cam.front = glm::normalize(direction);
    
    view = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);


    // MVP Composite

    // local -> world space
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // don't need model, stars already in world space
    
    // view space -> clip space
    glm::mat4 projection;
    // glm::perspective Parameters: FOV, Aspect Ratio (w/h), near plane, far plane
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.05f, 100.0f);

    glm::mat4 mvp_composite = projection * view;
    int compositeLoc = glGetUniformLocation(shader_program.ID, "mvp_composite");
    glUniformMatrix4fv(compositeLoc, 1, GL_FALSE, glm::value_ptr(mvp_composite));

    // Depth On
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive — stars glow through each other
    glDepthMask(GL_FALSE); // Prevent z-fighting at the far end of the scene


    // Controls
    bool pause = false;
    bool space_was_pressed = false;

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide mouse cursor
    glfwSetCursorPosCallback(window, processMouseInput);

    // Frame time
    float delta_time = 0.0f;
    float last_frame_time = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float now = (float)glfwGetTime();
        delta_time = now - last_frame_time;
        last_frame_time = now;

        // Controls
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


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        
        processInput(window);
        
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cam.front = glm::normalize(direction);
        
        processCameraInput(window, cam, delta_time);
        view = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
                
        // Clear depth and color buffer on each loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        
        shader_program.use();
        
        
        
        // Update MVP Composite
        glm::mat4 mvp_composite = projection * view;
        glUniformMatrix4fv(compositeLoc, 1, GL_FALSE, glm::value_ptr(mvp_composite));
        
        // Triangle
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_POINTS, 0, stars.size());

        ImGui::Begin("Debug");
        ImGui::Text("Cam: %.2f %.2f %.2f", cam.pos.x, cam.pos.y, cam.pos.z);
        ImGui::Text("Yaw: %.1f  Pitch: %.1f", yaw, pitch);
        ImGui::Text("Stars: %zu", stars.size());
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }  

    std::cout << "Terminating." << std::endl;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}


