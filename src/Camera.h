#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <vector>
#include "Debugger.h"
#include "Shader.h"
#include "RubiksCube.h"

class Camera
{
public:
    // Matrices
    glm::mat4 m_View = glm::mat4(1.0f);
    glm::mat4 m_Projection = glm::mat4(1.0f);

    // Rubik's Cube reference (holds cube data and behavior)
    RubiksCube& rubiksCube;

    // Camera transformation parameters
    glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f); // Forward vector
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);           // Up vector

    // Perspective parameters
    float m_FOV = 45.0f;
    float m_PerspectiveNear = 0.1f;
    float m_PerspectiveFar = 100.0f;

    // Orthographic parameters
    float m_Near = 0.1f;
    float m_Far = 100.0f;
    float m_Left = -1.0f;
    float m_Right = 1.0f;
    float m_Bottom = -1.0f;
    float m_Top = 1.0f;

    // Viewport dimensions
    int m_Width;
    int m_Height;

    // Mouse state variables
    double m_OldMouseX = 0.0;
    double m_OldMouseY = 0.0;

    // Rotation angles (for pitch and yaw)
    float m_RotationAngleX = 0.0f;
    float m_RotationAngleY = 0.0f;

    // Mouse rotation sensitivity
    float m_RotationSensitivity = 0.1f;

    // Constructor
    Camera(int width, int height, RubiksCube& cubeRef)
            : m_Width(width), m_Height(height), rubiksCube(cubeRef) {}

    // Projection setup methods
    void SetOrthographic(float nearPlane, float farPlane);
    void SetPerspective(float fov, float nearPlane, float farPlane);
    void SetPosition(glm::vec3 newPosition);

    // Input and view update methods
    void EnableInputs(GLFWwindow* window);
    void UpdateViewMatrix();
    inline glm::mat4 GetViewMatrix() const { return m_View; }
    inline glm::mat4 GetProjectionMatrix() const { return m_Projection; }

    // Rubik's Cube interaction handlers
    void handleRKey();
    void handleLKey();
    void handleUKey();
    void handleDKey();
    void handleBKey();
    void handleFKey();
    void handleSpaceKey();
    void handleZKey();
    void handleAKey();

    // Arrow key handlers
    void handleUpArrow();
    void handleDownArrow();
    void handleLeftArrow();
    void handleRightArrow();

    // Picking mode handler
    void handlePKey();

    // Mixer bonus handler
    void handleMKey();
};
