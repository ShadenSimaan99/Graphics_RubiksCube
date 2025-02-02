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
#include <RubiksCube.h>
#include <iostream>

/* Window size */
const unsigned int width = 800;
const unsigned int height = 800;
const float FOVdegree = 45.0f;  // Field Of View Angle
const float near = 0.1f;
const float far = 100.0f;

/* Cube vertices (common mesh) */
float vertices[] = {
    // positions            // colors            // texCoords
    // Back face
    -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,
    // Front face
    -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
    // Left face
    -0.5f, -0.5f,  0.5f,    1.0f, 0.5f, 0.0f,    1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    1.0f, 0.5f, 0.0f,    0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 0.5f, 0.0f,    0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 0.5f, 0.0f,    1.0f, 1.0f,
    // Right face
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
    // Down face
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
    // Up face
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f
};

unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,       // back face
    4, 5, 6, 6, 7, 4,       // front face
    8, 9, 10, 10, 11, 8,    // left face
    12, 13, 14, 14, 15, 12, // right face
    16, 17, 18, 18, 19, 16, // down face
    20, 21, 22, 22, 23, 20  // up face
};

int main(int argc, char* argv[])
{
    GLFWwindow* window;
    if (!glfwInit())
    {
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(width, height, "OpenGL Rubik's Cube", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    
    {
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    
        VertexArray va;
        VertexBuffer vb(vertices, sizeof(vertices));
        IndexBuffer ib(indices, sizeof(indices));
    
        VertexBufferLayout layout;
        layout.Push<float>(3);  // positions
        layout.Push<float>(3);  // colors
        layout.Push<float>(2);  // texCoords
        va.AddBuffer(vb, layout);
    
        Texture texture("res/textures/plane.png");
        texture.Bind();
    
        Shader shader("res/shaders/basic.shader");
        shader.Bind();
    
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();
    
        // Instantiate RubiksCube (global instance)
        RubiksCube rubiksCube;
    
        // Create camera and pass rubiksCube by reference
        Camera camera(width, height, rubiksCube);
        camera.SetPerspective(FOVdegree, near, far);
        camera.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
        camera.EnableInputs(window);
    
        while (!glfwWindowShouldClose(window))
        {
            GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 proj = camera.GetProjectionMatrix();
    
            rubiksCube.render(shader, va, ib, proj, view, window);
    
            glfwPollEvents();
        }
    }
    
    glfwTerminate();
    return 0;
}
