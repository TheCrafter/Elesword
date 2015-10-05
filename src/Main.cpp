#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "Shader.hpp"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

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

    Shader prog;
    prog.Init("res/Shader/Vertex/shader.vs", "res/Shader/Fragment/shader.frag");
    prog.Use();

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
        // Positions       // Colors         // Texture Coords
        0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // Top Right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // Bottom Right
       -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Bottom Left
       -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // Top Left
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,   // First Triangle
        1, 2, 3    // Second Triangle
    };

    GLuint VBO, EBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(
            0,                      // The vertex attribute we want to configure. On shader we used layout (position = 0)!
            4,                      // Size of vertex attribute. vec3 = 3
            GL_FLOAT,               // type of data
            GL_FALSE,               // do we want the data to be normalized?
            8 * sizeof(GLfloat),    // stride: The space between consecutive vertex attribute sets
            (GLvoid*)0);            // offset where position data begins in buffer
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            1,                                  // The vertex attribute we want to configure. On shader we used layout (position = 0)!
            4,                                  // Size of vertex attribute. vec3 = 3
            GL_FLOAT,                           // type of data
            GL_FALSE,                           // do we want the data to be normalized?
            8 * sizeof(GLfloat),                // stride: The space between consecutive vertex attribute sets
            (GLvoid*)(3 * sizeof(GLfloat)));    // offset where position data begins in buffer
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            2,                                  // The vertex attribute we want to configure. On shader we used layout (position = 0)!
            2,                                  // Size of vertex attribute. vec3 = 3
            GL_FLOAT,                           // type of data
            GL_FALSE,                           // do we want the data to be normalized?
            8 * sizeof(GLfloat),                // stride: The space between consecutive vertex attribute sets
            (GLvoid*)(6 * sizeof(GLfloat)));    // offset where position data begins in buffer
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    GLuint textures[2];
    int width, height;
    unsigned char* image;
    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
        image = SOIL_load_image("res/Image/container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
        image = SOIL_load_image("res/Image/awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);	// Activate the texture unit first before binding texture
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glUniform1i(glGetUniformLocation(prog.GetProgID(), "texture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glUniform1i(glGetUniformLocation(prog.GetProgID(), "texture2"), 1);

        glUseProgram(prog.GetProgID());

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    (void*)scancode;
    (void*)mode;

    // When a user presses the escape key, we set the WindowShouldClose property to true, 
    // closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}