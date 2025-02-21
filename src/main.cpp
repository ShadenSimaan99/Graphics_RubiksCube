#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>
#include <Camera.h>
#include <SmallCube.h>
#include <RubiksCube.h>
#include <iostream>

// Global Rubik's Cube instance.
RubiksCube rubiksCube;

// Vertex data for a textured cube (positions, colors, and texture coordinates).
float vertices[] = {
        // Front face (Red)
        -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left

        // Back face (Orange)
        -0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 1.0f,

        // Left face (Green)
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

        // Right face (Blue)
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

        // Top face (White)
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // Bottom face (Yellow)
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
};

// Indices for drawing each cube face as two triangles.
unsigned int indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
};

int main() {
    // Window and perspective parameters.
    const unsigned int WIN_WIDTH  = 800;
    const unsigned int WIN_HEIGHT = 600;
    const float FOV = 45.0f;
    const float NEAR_PLANE = 0.1f;
    const float FAR_PLANE  = 100.0f;

    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed." << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window.
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL Rubik's Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1); // Enable VSync

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Enable depth testing.
    GLCall(glEnable(GL_DEPTH_TEST));

    // Set up the vertex array, vertex buffer, index buffer, and layout.
    VertexArray vao;
    VertexBuffer vbo(vertices, sizeof(vertices));
    IndexBuffer ibo(indices, sizeof(indices));
    VertexBufferLayout layout;
    layout.Push<float>(3); // Positions
    layout.Push<float>(3); // Colors
    layout.Push<float>(2); // Texture coordinates
    vao.AddBuffer(vbo, layout);

    // Load and bind the texture.
    Texture texture("res/textures/plane.png");
    texture.Bind();

    // Initialize and bind the shader.
    Shader shader("res/shaders/basic.shader");
    shader.Bind();

    // Unbind everything for now.
    vao.Unbind();
    vbo.Unbind();
    ibo.Unbind();
    shader.Unbind();

    // Initialize the camera and configure its perspective and position.
    Camera camera(WIN_WIDTH, WIN_HEIGHT, rubiksCube);
    camera.SetPerspective(FOV, NEAR_PLANE, FAR_PLANE);
    camera.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    camera.EnableInputs(window);

    // Main render loop.
    while (!glfwWindowShouldClose(window)) {
        GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f)); // White background.
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        glm::mat4 viewMatrix = camera.GetViewMatrix();
        glm::mat4 projMatrix = camera.GetProjectionMatrix();

        // Render the Rubik's Cube.
        rubiksCube.render(shader, vao, ibo, projMatrix, viewMatrix, window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
