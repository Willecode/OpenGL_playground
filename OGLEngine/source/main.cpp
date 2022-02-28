#include <glad/glad.h>
#include <GLFW/glfw3.h>
//--------------------------------------
// IMAGE LOADER
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
//--------------------------------------
// MATH FUNCTIONS
#define GLM_ENABLE_EXPERIMENTAL // for glm/ext.hpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp> // For printing glm vectors
//--------------------------------------
// STRING FORMATTING
#define FMT_HEADER_ONLY
#include <fmt/core.h>
//--------------------------------------
#include "shader_s.h"
#include "scene.h"
#include "object3d.h"
#include "texture.h"
#include "materials/phongmaterial.h"
#include "materials/coloronlymaterial.h"
#include "primitivecreation.h"
#include "camera.h"

#include <iostream>
#include <map>
#include <array>
#include <vector>


void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void getUniformLocations(std::map<std::string, int>* m, std::vector<std::string> names, int shaderID);
void mouseCallback(GLFWwindow* window, double xPosArg, double yPosArg);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;
float fov = 45.f;


Camera camera;
bool firstMouse = true;
float lastMouseX = SCR_WIDTH / 2.0f;
float lastMouseY = SCR_HEIGHT / 2.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "�Wilkan Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Create primitive meshes
    const Mesh MESH_CUBE = createCubeMesh();

    // Compile shaders
    Shader phongTexShader("shaders/object.vs", "shaders/phongtex.fs");
    Shader phongSolidColShader("shaders/object.vs", "shaders/phongsolidcol.fs");
    Shader colorOnlyShader("shaders/object.vs", "shaders/coloronly.fs");
    glEnable(GL_DEPTH_TEST);

    stbi_set_flip_vertically_on_load(true);

    // Set up textures
    ImageCache cache;
    Texture diffuseMap("images/Wood066_1K_Color.jpg", GL_TEXTURE_2D, cache);
    Texture specularMap("images/Fingerprints009_1K_Color.jpg", GL_TEXTURE_2D, cache);
    
    Scene scene;
    
    glm::vec3 colorPink = glm::vec3(232.0f / 255, 100.0f / 255, 190.0f / 255);
    //PhongMaterial cubeMat;
    
    // Create objects
    PhongMaterial cubeMat(&diffuseMap, &specularMap);
    PhongMaterial cubeMat2;
    Object3D cube;
    cube.addMesh(&MESH_CUBE);
    cube.setMaterial(&cubeMat2);
    cube.setShader(&phongSolidColShader);

    glm::vec3 lampCol = glm::vec3(0.f, 153.f / 255.f, 0.f);
    PointLight light;

    Object3D dirLightObj;
    DirectionalLight dirLight(glm::vec3(-0.5f, -0.5f, 0.0f));
    dirLightObj.addLightSource(&dirLight);

    Object3D lamp;
    ColorOnlyMaterial lampMat(lampCol);
    lamp.addMesh(&MESH_CUBE);
    lamp.setMaterial(&lampMat);
    lamp.setShader(&colorOnlyShader);
    glm::mat4 lampModelMat = glm::mat4(1.0f);
    lampModelMat = glm::translate(lampModelMat, glm::vec3(2.0f, 0.0f, 0.0f));
    lamp.setModelMatrix(lampModelMat);
    lamp.addLightSource(&light);

    Object3D lamp2 = lamp;
    glm::mat4 lamp2ModelMat = glm::mat4(1.0f);
    lamp2ModelMat = glm::translate(lamp2ModelMat, glm::vec3(-5.0f, .0f, 0.0f));
    lamp2.setModelMatrix(lamp2ModelMat);

    // Add objects to scene
    //scene.addObject(&lamp);
    scene.addObject(&lamp2);
    scene.addObject(&cube);
    //scene.addObject(&dirLightObj);

    scene.updateLighting();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        // -----
        processInput(window);

        //glm::mat4 viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.getFov()), 1000.0f / 800.0f, 0.1f, 100.0f);

        phongSolidColShader.use();
        glUniformMatrix4fv(glGetUniformLocation(phongSolidColShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(phongSolidColShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        phongSolidColShader.setFloat3("viewPos", camera.position);
        colorOnlyShader.use();
        glUniformMatrix4fv(glGetUniformLocation(colorOnlyShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(colorOnlyShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        colorOnlyShader.setFloat3("viewPos", camera.position);


        lamp2ModelMat = glm::mat4(1.0f);
        lamp2ModelMat = glm::translate(lamp2ModelMat, glm::vec3(sin((float)glfwGetTime()) * 2, 0.0f, cos((float)glfwGetTime()) * 2));
        lamp2ModelMat = glm::scale(lamp2ModelMat, glm::vec3(0.2f, 0.2f, 0.2f));
        lamp2.setModelMatrix(lamp2ModelMat);

        scene.drawScene();
       
        // glfw: swap buffers and poll IO events
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.position += cameraSpeed * camera.direction;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.position -= cameraSpeed * camera.direction;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.position -= glm::normalize(glm::cross(camera.direction, camera.worldUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.position += glm::normalize(glm::cross(camera.direction, camera.worldUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.position += camera.worldUp * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.position += -camera.worldUp * cameraSpeed;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double mouseXIn, double mouseYIn) {
    float mouseX = static_cast<float>(mouseXIn);
    float mouseY = static_cast<float>(mouseYIn);

    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }
    camera.processMouseMovement(mouseX - lastMouseX, mouseY - lastMouseY);
    lastMouseX = mouseX;
    lastMouseY = mouseY;

}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    float fov = camera.getFov();
    fov += static_cast<float>(-yoffset);
    camera.setFov(fov);
}

void getUniformLocations(std::map<std::string, int>* m, std::vector<std::string> names, int shaderID) {
    // Populates m with argument name:location
    for (int i = 0; i < names.size(); i++) {
        m->insert(std::pair<std::string, int>(names.at(i), glGetUniformLocation(shaderID, names.at(i).c_str())));
    }
}