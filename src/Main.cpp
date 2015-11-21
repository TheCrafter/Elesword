#include "Util/WarnGuard.hpp"

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

#include "Camera.hpp"
#include "Movement.hpp"
#include "Shader.hpp"
#include "Model/Model.hpp"
#include "Model/AssimpLoader.hpp"
#include "Model/SimpleLoader.hpp"

//-----------------------------------------------------
// Data
//-----------------------------------------------------
// System
GLFWwindow* window;
GLsizei width = 800, height = 600;

GLenum polygonMode = GL_FILL;

// World
Camera* worldCam;

// Shaders
Shader lightingShader, lampShader;

// Models
struct World
{
    // Camera
    Camera camera;

    // Models
    std::unique_ptr<Model<AssimpLoader, AssimpPainter, AssimpMesh>> nanosuit;
    std::unique_ptr<Model<SimpleLoader, SimplePainter, SimpleMesh>> lamp1;
    std::unique_ptr<Model<SimpleLoader, SimplePainter, SimpleMesh>> lamp2;

    // Other matrices
    glm::mat4 view, proj;

    // Light attributes
    glm::vec3 lightPos;

    std::array<glm::vec3, 2> pointLightPositions;

    World()
        : lightPos(1.2f, 1.0f, 2.0f)
        , camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
        , proj(glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f))
    {
        pointLightPositions[0] = glm::vec3(2.3f, -1.6f, -3.0f);
        pointLightPositions[1] = glm::vec3(-1.7f, 0.9f, 1.0f);
    }

};


bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
//-----------------------------------------------------

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    (void*)scancode;
    (void*)mode;

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Draw in wireframe
    if(key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        (polygonMode == GL_FILL) ? polygonMode = GL_LINE : polygonMode = GL_FILL;
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    }

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

    worldCam->RotateCamera(xoffset, yoffset);
}

inline void doMovement(World& world, GLfloat deltaTime)
{
    // Camera controls
    GLfloat cameraSpeed = 5.0f;
    GLfloat distance = cameraSpeed * deltaTime;
    if(keys[GLFW_KEY_W])
        world.camera.MoveCamera(Movement::MoveDirection::In, distance);
    if(keys[GLFW_KEY_S])
        world.camera.MoveCamera(Movement::MoveDirection::Out, distance);
    if(keys[GLFW_KEY_A])
        world.camera.MoveCamera(Movement::MoveDirection::Left, distance);
    if(keys[GLFW_KEY_D])
        world.camera.MoveCamera(Movement::MoveDirection::Right, distance);

    GLfloat modelSpeed = 2.0f;
    if(keys[GLFW_KEY_UP])
        world.nanosuit->Move<Movement::MoveDirection::Up>(modelSpeed);

    if(keys[GLFW_KEY_DOWN])
        world.nanosuit->Move<Movement::MoveDirection::Down>(modelSpeed);

    if(keys[GLFW_KEY_RIGHT])
        world.nanosuit->Move<Movement::MoveDirection::Right>(modelSpeed);

    if(keys[GLFW_KEY_LEFT])
        world.nanosuit->Move<Movement::MoveDirection::Left>(modelSpeed);
}

GLFWwindow* CreateContext()
{
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
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
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
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }

    // Define the viewport dimensions
    glViewport(0, 0, width, height);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    return window;
}

void Update(World& world, float deltaTime)
{
    glfwPollEvents();
    doMovement(world, deltaTime);

    // Camera
    world.view = world.camera.GetView();
}

void Render(const World& world)
{
    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);     // blue(ish)
    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);    // gray
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.GetProgID(), "projection"), 1, GL_FALSE, glm::value_ptr(world.proj));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.GetProgID(), "view"), 1, GL_FALSE, glm::value_ptr(world.view));

    // Set the lighting uniforms
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "viewPos"), world.camera.mCameraPos.x, world.camera.mCameraPos.y, world.camera.mCameraPos.z);
    // Point light 1
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].position"), world.pointLightPositions[0].x, world.pointLightPositions[0].y, world.pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].linear"), 0.009f);
    glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[0].quadratic"), 0.0032f);
    // Point light 2
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].position"), world.pointLightPositions[1].x, world.pointLightPositions[1].y, world.pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].linear"), 0.009f);
    glUniform1f(glGetUniformLocation(lightingShader.GetProgID(), "pointLights[1].quadratic"), 0.0032f);

    // Draw the loaded model
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.GetProgID(), "model"), 1, GL_FALSE, glm::value_ptr(world.nanosuit->GetModelMat()));
    world.nanosuit->Draw(lightingShader);

    //---
    // Lamp
    //---
    lampShader.Use();
    GLint modelLoc = glGetUniformLocation(lampShader.GetProgID(), "model"),
          viewLoc = glGetUniformLocation(lampShader.GetProgID(), "view"),
          projLoc = glGetUniformLocation(lampShader.GetProgID(), "projection");

    // Pass matrices to shader (except model for now_
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(world.view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(world.proj));

    // Draw Lamps
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(world.lamp1->GetModelMat()));
    world.lamp1->Draw(lampShader);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(world.lamp2->GetModelMat()));
    world.lamp2->Draw(lampShader);

    // Swap the screen buffers
    glfwSwapBuffers(window);
}

int main()
{
#ifdef _WIN32
    //FreeConsole();
#endif

    window = CreateContext();
    if(window == nullptr)
    {
        std::cerr << "Failed to create OpenGL context." << std::endl;
        return -1;
    }

    World world;
    worldCam = &world.camera;

    // Load shaders
    lightingShader.Init("res/Shader/Vertex/lighting.vert", "res/Shader/Fragment/lighting.frag");
    lampShader.Init("res/Shader/Vertex/lamp.vert", "res/Shader/Fragment/lamp.frag");

    // Load models
    std::shared_ptr<AssimpLoader> assimpLoader = std::make_shared<AssimpLoader>();
    std::shared_ptr<AssimpPainter> assimpPainter = std::make_shared<AssimpPainter>();
    world.nanosuit = std::make_unique<Model<AssimpLoader, AssimpPainter, AssimpMesh>>("res/Model/Nanosuit/nanosuit.obj", assimpLoader, assimpPainter);
    world.nanosuit->Load();

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

    world.lamp1 = std::make_unique<Model<SimpleLoader, SimplePainter, SimpleMesh>>("res/Model/Lamp/lamp.obj", sLoader, sPainter);
    world.lamp2 = std::make_unique<Model<SimpleLoader, SimplePainter, SimpleMesh>>("res/Model/Lamp/lamp.obj", sLoader, sPainter);
    world.lamp1->Load();
    world.lamp2->Load();

    GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
    GLfloat lastFrame = 0.0f;  	// Time of last frame

    world.nanosuit->Translate(glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
    world.nanosuit->Scale(glm::vec3(0.2f, 0.2f, 0.2f));       // It's a bit too big for our scene, so scale it down

    world.lamp1->Translate(world.pointLightPositions[0]);   // Move it to its position
    world.lamp1->Scale(glm::vec3(0.2f));              // Make it a smaller cube

    world.lamp2->Translate(world.pointLightPositions[1]);   // Move it to its position
    world.lamp2->Scale(glm::vec3(0.2f));              // Make it a smaller cube

    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        Update(world, deltaTime);
        Render(world);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
