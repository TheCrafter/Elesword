#include "Util/WarnGuard.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <memory>
#include <functional>

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
#include "Model/Model.hpp"
#include "Model/AssimpLoader.hpp"
#include "Render/Light.hpp"
#include "Render/Shader.hpp"

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
Shader lightingShader, lampShader, singleColorShader;

// Models
struct World
{
    // Camera
    Camera camera;

    // Models
    std::unique_ptr<Model> nanosuit, nanosuit2, lamp1, lamp2;

    // Other matrices
    glm::mat4 view, proj;

    // Light attributes
    PointLight pointLights[2];

    World()
        : camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
        , proj(glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f))
    {
        // Set first light
        pointLights[0].attr.position  = glm::vec3(2.3f, -1.6f, -3.0f);
        pointLights[0].ambient        = glm::vec3(0.05f, 0.05f, 0.05f);
        pointLights[0].diffuse        = glm::vec3(1.0f,  1.0f,  1.0);
        pointLights[0].specular       = glm::vec3(1.0f,  1.0f,  1.0);
        pointLights[0].attr.constant  = 1.0f;
        pointLights[0].attr.linear    = 0.009f;
        pointLights[0].attr.quadratic = 0.0032f;

        // Set second light
        pointLights[1].attr.position  = glm::vec3(-1.7f, 0.9f, 1.0f);
        pointLights[1].ambient        = glm::vec3(0.05f, 0.05f, 0.05f);
        pointLights[1].diffuse        = glm::vec3(1.0f,  1.0f,  1.0);
        pointLights[1].specular       = glm::vec3(1.0f,  1.0f,  1.0);
        pointLights[1].attr.constant  = 1.0f;
        pointLights[1].attr.linear    = 0.009f;
        pointLights[1].attr.quadratic = 0.0032f;
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
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilMask(0x00);

    // Setup Shaders
        // lightingShader
    {
        lightingShader.Use();
        GLuint id = lightingShader.GetProgID();
        lightingShader.LoadView(world.view);
        lightingShader.LoadProjection(world.proj);

        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(id, "viewPos"), world.camera.mCameraPos.x, world.camera.mCameraPos.y, world.camera.mCameraPos.z);

        // Load point lights to GPU
        LoadLight<PointLight>(id, world.pointLights[0], "pointLights[0]");
        LoadLight<PointLight>(id, world.pointLights[1], "pointLights[1]");
    }
        // singleColorShader
    {
        singleColorShader.Use();
        GLuint id = singleColorShader.GetProgID();
        singleColorShader.LoadView(world.view);
        singleColorShader.LoadProjection(world.proj);

        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(id, "viewPos"), world.camera.mCameraPos.x, world.camera.mCameraPos.y, world.camera.mCameraPos.z);

        // Load point lights to GPU
        LoadLight<PointLight>(id, world.pointLights[0], "pointLights[0]");
        LoadLight<PointLight>(id, world.pointLights[1], "pointLights[1]");
    }
        // lampShader
    {
        lampShader.Use();
        lampShader.LoadView(world.view);
        lampShader.LoadProjection(world.proj);
    }

    // Draw models
    world.nanosuit->Render(lightingShader);
    world.nanosuit->RenderOutline(singleColorShader);
    world.nanosuit2->Render(lightingShader);
    world.lamp1->Render(lampShader);
    world.lamp2->Render(lampShader);

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
    singleColorShader.Init("res/Shader/Vertex/singleColor.vert", "res/Shader/Fragment/singleColor.frag");

    // Create Models
    std::unique_ptr<AssimpLoader> assimpLoader = std::make_unique<AssimpLoader>();
    std::unique_ptr<AssimpPainter> assimpPainter = std::make_unique<AssimpPainter>();

    // Load data
    std::unique_ptr<ModelData> nanosuitData(assimpLoader->LoadData("res/Model/Nanosuit/nanosuit.obj"));
    std::unique_ptr<ModelData> lampData(assimpLoader->LoadData("res/Model/Lamp/lamp.obj"));

    Model::RenderMeshCb rmcb = std::bind(
        &AssimpPainter::DrawMesh,
        assimpPainter.get(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3);

    world.nanosuit = Model::CreateModel(nanosuitData.get(), rmcb);
    world.nanosuit2 = Model::CreateModel(nanosuitData.get(), rmcb);
    world.lamp1 = Model::CreateModel(lampData.get(), rmcb);
    world.lamp2 = Model::CreateModel(lampData.get(), rmcb);

    GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
    GLfloat lastFrame = 0.0f;  	// Time of last frame

    world.nanosuit->Translate(glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
    world.nanosuit->Scale(glm::vec3(0.2f, 0.2f, 0.2f));       // It's a bit too big for our scene, so scale it down

    world.nanosuit2->Translate(glm::vec3(-3.0f, -1.75f, 0.0f));  // Translate it down a bit so it's at the center of the scene
    world.nanosuit2->Scale(glm::vec3(0.2f, 0.2f, 0.2f));         // It's a bit too big for our scene, so scale it down

    world.lamp1->Translate(world.pointLights[0].attr.position);   // Move it to its position
    world.lamp1->Scale(glm::vec3(0.2f));                          // Make it a smaller cube

    world.lamp2->Translate(world.pointLights[1].attr.position);   // Move it to its position
    world.lamp2->Scale(glm::vec3(0.2f));                          // Make it a smaller cube

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
