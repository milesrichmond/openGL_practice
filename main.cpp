#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <shader.h>
#include <iostream>
#include <cmath>

// Simple user input
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// Handles user resizing of window
// Called by GLFW whenever window size changes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialization
    // GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFW window declaration & configuration
    GLFWwindow* window = glfwCreateWindow(800, 600, "Satallite", NULL, NULL);
    if(window == NULL) {
        std::cout << "failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLAD init
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Configures callback for window resizes

    Shader shade("../shaders/vertexShader.vert", "../shaders/fragmentShader.frag");

    // Rectangle vertices (EBOs will reduce the repeated vertices)
    float vertices[] = {
        0.5f,  0.5f, 0.0f, /**/ 1.0f, 0.0f, 0.0f, /**/ 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, /**/ 0.0f, 1.0f, 0.0f, /**/ 1.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f, /**/ 1.0f, 1.0f, 1.0f, /**/ 0.0f, 0.0f, // bottom left
       -0.5f,  0.5f, 0.0f, /**/ 0.0f, 0.0f, 1.0f, /**/ 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    // Buffer & Array Object setup
    // Vertex Buffer Object, reduces the traffic from CPU to GPU
    // Vertex Array Object, stores the config below to simplify future calls
    // Element Buffer Object, allows indexed drawing (tells OpenGL what order to render in in order to reduce the number of vertices stored)
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // sets VBO to the ID for the buffer
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); // Bound first before VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Future calls targeting the GL_ARRAY_BUFFER will target the currently bound buffer (VBO in this case)

    // The 4th argument specifies how the GPU should handle the data, 3 options:    GL_STREAM_DRAW  : sent once, used a few times
    //                                                                              GL_STATIC_DRAW  : sent once, used lots
    //                                                                              GL_DYNAMIC_DRAW : data is changing, used lots
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies the vertex data into buffer memory

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Arg explanation: 1: which vertex attribute to configure (with these simple shaders, the aPos vertex attribute was set to location 0, hence the passed argument)
    //                  2: size of the vertex attribute. vec3 has 3 values
    //                  3: data type
    //                  4: should the data be normalized? (not relavant in our situation, thus set to GL_FALSE)
    //                  5: stride (the space between vertex attributes.) Each set of position data is 3 floats away (xyzxyzxyz)
    //                  6: position data offset from the beginning of the buffer. (icky cast to void*)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // vertex attributes are disabled by default. This function is passed the attribute location

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture attrib
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Texture
    stbi_set_flip_vertically_on_load(true);
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // loading image
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../textures/wall.jpg", &width, &height, &nrChannels, 0);

    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" 
        << std::endl;
    }
    stbi_image_free(data);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("../textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    shade.use();
    shade.setInt("currentTexture2", 1);

    // Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Render Loop
    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f,0.3f,0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // Clears the buffer for the next frame and prevents previous results from being displayed

        shade.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(VAO); // indirectly binds the VBO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    shade.deleteShader();

    glfwTerminate();
    return 0;
}