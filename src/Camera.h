#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "RubiksCube.h"
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <utility>

class Camera {
public:
    glm::mat4 m_View = glm::mat4(1.0f);
    glm::mat4 m_Projection = glm::mat4(1.0f);

    glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    int m_Width, m_Height;
    float m_FOV = 45.0f;
    float m_PerspectiveNear = 0.1f;
    float m_PerspectiveFar = 100.0f;

    // For mouse motion:
    double m_OldMouseX = 0.0, m_OldMouseY = 0.0;
    float m_RotationSensitivity = 0.1f;

    // Reference to the RubiksCube instance.
    RubiksCube& rubiksCube;

    Camera(int width, int height, RubiksCube& rubiksCube)
      : m_Width(width), m_Height(height), rubiksCube(rubiksCube) {
        m_Projection = glm::perspective(glm::radians(m_FOV), (float)width / m_Height, m_PerspectiveNear, m_PerspectiveFar);
        m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
    }
    
    inline glm::mat4 GetViewMatrix() const { return m_View; }
    inline glm::mat4 GetProjectionMatrix() const { return m_Projection; }
    
    void TranslateProjection(glm::vec3 trans) {
        m_Projection = glm::translate(m_Projection, trans);
    }
    
    void SetPerspective(float fov, float near, float far) {
        m_FOV = fov;
        m_PerspectiveNear = near;
        m_PerspectiveFar = far;
        m_Projection = glm::perspective(glm::radians(m_FOV), (float)m_Width / m_Height, near, far);
    }
    
    void SetPosition(glm::vec3 pos) {
        m_Position = pos;
        UpdateViewMatrix();
    }
    
    void UpdateViewMatrix() {
        m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
    }
    
    // Enable input callbacks.
    void EnableInputs(GLFWwindow* window);

    // --- Key handler functions ---
    void handleRKey();
    void handleLKey();
    void handleUKey();
    void handleDKey();
    void handleBKey();
    void handleFKey();
    void handleSpaceKey();
    void handleZKey();
    void handleAKey();
    void handlePKey();
    void handleMKey();
    void handleUpArrow();
    void handleDownArrow();
    void handleLeftArrow();
    void handleRightArrow();
};

// Callback declarations:
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
