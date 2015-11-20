#include "WarnGuard.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
WARN_GUARD_OFF

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "AssimpLoader.hpp"
#include "SimpleLoader.hpp"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void doMovement(GLfloat deltaTime);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

int main()
{
#ifdef _WIN32
    //FreeConsole();
#endif

    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Define the viewport dimensions
    glViewport(0, 0, 800, 600);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Load shaders
    Shader lightingShader;
    lightingShader.Init("res/Shader/Vertex/lighting.vert", "res/Shader/Fragment/lighting.frag");
    Shader lampShader;
    lampShader.Init("res/Shader/Vertex/lamp.vert", "res/Shader/Fragment/lamp.frag");

    // Load model
    std::shared_ptr<AssimpLoader> assimpLoader = std::make_shared<AssimpLoader>();
    std::shared_ptr<AssimpPainter> assimpPainter = std::make_shared<AssimpPainter>();
    Model<AssimpLoader, AssimpPainter, AssimpMesh> nanosuit("res/Model/Nanosuit/nanosuit.obj", assimpLoader, assimpPainter);
    nanosuit.Load();

    // Create vertices and indices for our cute cube lamp
    std::vector<GLfloat> lampVertices = {
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };

    std::vector<GLuint> lampIndices = {
        0, 1, 2, 2, 3, 0,
        3, 2, 6, 6, 7, 3,
        7, 6, 5, 5, 4, 7,
        4, 0, 3, 3, 7, 4,
        0, 1, 5, 5, 4, 0,
        1, 5, 6, 6, 2, 1
    };

    std::shared_ptr<SimpleLoader> sLoader = std::make_shared<SimpleLoader>();
    std::shared_ptr<SimplePainter> sPainter = std::make_shared<SimplePainter>();
    sLoader->GetVertices().insert(sLoader->GetVertices().end(), lampVertices.begin(), lampVertices.end());
    sLoader->GetIndices().insert(sLoader->GetIndices().end(), lampIndices.begin(), lampIndices.end());

    Model<SimpleLoader, SimplePainter, SimpleMesh> lamp("res/Model/Lamp/lamp.obj", sLoader, sPainter);
    lamp.Load();

    // Draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set up vertex data (and buffer(s)) and attribute pointers

    // Point light positions
    glm::vec3 pointLightPositions[] = {
        glm::vec3(2.3f, -1.6f, -3.0f),
        glm::vec3(-1.7f, 0.9f, 1.0f)
    };

    GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
    GLfloat lastFrame = 0.0f;  	// Time of last frame

    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed, mouse moved etc.)
        // and call corresponding response functions
        glfwPollEvents();
        doMovement(deltaTime);

        // Render
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // blue(ish)
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);   // gray
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create matrices
        glm::mat4 model, view, proj;

        // Camera
        view = camera.GetView();
        proj = glm::perspective(45.0f, (GLfloat)800 / (GLfloat)600, 0.1f, 100.0f);

        lightingShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.GetProgID(), "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.GetProgID(), "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "viewPos"), camera.mCameraPos.x, camera.mCameraPos.y, camera.mCameraPos.z);
        // Point light 1
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].linear"), 0.009f);
        glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].quadratic"), 0.0032f);
        // Point light 2
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].linear"), 0.009f);
        glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].quadratic"), 0.0032f);

        // Draw the loaded model
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.GetProgID(), "model"), 1, GL_FALSE, glm::value_ptr(model));

        nanosuit.Draw(lightingShader);

        //---
        // Lamp
        //---
        lampShader.Use();
        GLint modelLoc = glGetUniformLocation(lampShader.GetProgID(), "model");
        GLint viewLoc = glGetUniformLocation(lampShader.GetProgID(), "view");
        GLint projLoc = glGetUniformLocation(lampShader.GetProgID(), "projection");

        // Pass matrices to shader (except model for now_
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        // Draw
        for(GLuint i = 0; i < 2; i++)
        {
            model = glm::mat4();
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            lamp.Draw(lampShader);
        }

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    (void*)scancode;
    (void*)mode;

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if(key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    (void*)window;

    if(firstMouse)
    {
        lastX = (GLfloat)xpos;
        lastY = (GLfloat)ypos;
        firstMouse = false;
    }

    GLfloat xoffset = (GLfloat)xpos - lastX;
    GLfloat yoffset = lastY - (GLfloat)ypos;
    lastX = (GLfloat)xpos;
    lastY = (GLfloat)ypos;

    camera.RotateCamera(xoffset, yoffset);
}

void doMovement(GLfloat deltaTime)
{
    // Camera controls
    GLfloat cameraSpeed = 5.0f;
    GLfloat distance = cameraSpeed * deltaTime;
    if(keys[GLFW_KEY_W])
        camera.MoveCamera(Camera::MoveDirection::M_IN, distance);
    if(keys[GLFW_KEY_S])
        camera.MoveCamera(Camera::MoveDirection::M_OUT, distance);
    if(keys[GLFW_KEY_A])
        camera.MoveCamera(Camera::MoveDirection::M_LEFT, distance);
    if(keys[GLFW_KEY_D])
        camera.MoveCamera(Camera::MoveDirection::M_RIGHT, distance);
}
