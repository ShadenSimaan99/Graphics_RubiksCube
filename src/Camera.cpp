#include "Camera.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) return;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch(key) {
            case GLFW_KEY_UP:    camera->handleUpArrow(); break;
            case GLFW_KEY_DOWN:  camera->handleDownArrow(); break;
            case GLFW_KEY_LEFT:  camera->handleLeftArrow(); break;
            case GLFW_KEY_RIGHT: camera->handleRightArrow(); break;
            case GLFW_KEY_R:     camera->handleRKey(); break;
            case GLFW_KEY_L:     camera->handleLKey(); break;
            case GLFW_KEY_U:     camera->handleUKey(); break;
            case GLFW_KEY_D:     camera->handleDKey(); break;
            case GLFW_KEY_B:     camera->handleBKey(); break;
            case GLFW_KEY_F:     camera->handleFKey(); break;
            case GLFW_KEY_SPACE: camera->handleSpaceKey(); break;
            case GLFW_KEY_Z:     camera->handleZKey(); break;
            case GLFW_KEY_A:     camera->handleAKey(); break;
            case GLFW_KEY_P:     camera->handlePKey(); break;
            case GLFW_KEY_M:
                std::thread([camera]() { camera->handleMKey(); }).detach();
                break;
            default:
                break;
        }
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        std::cout << "MOUSE LEFT Click" << std::endl;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        std::cout << "MOUSE RIGHT Click" << std::endl;
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) return;
    float deltaX = camera->m_OldMouseX - xpos;
    float deltaY = camera->m_OldMouseY - ypos;
    camera->m_OldMouseX = xpos;
    camera->m_OldMouseY = ypos;
    
    // If left mouse button is pressed: rotate (as before)
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (camera->rubiksCube.pickingMode && camera->rubiksCube.selectedCube) {
            float angleY = deltaX * camera->m_RotationSensitivity;
            float angleX = deltaY * camera->m_RotationSensitivity;
            glm::mat4 transToOrigin = glm::translate(glm::mat4(1.0f), -camera->rubiksCube.selectedCube->getPosition());
            glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(angleX), glm::vec3(1,0,0));
            glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0,1,0));
            glm::mat4 transBack = glm::translate(glm::mat4(1.0f), camera->rubiksCube.selectedCube->getPosition());
            glm::mat4 final = transBack * rotY * rotX * transToOrigin;
            camera->rubiksCube.selectedCube->setModelMatrix(final * camera->rubiksCube.selectedCube->getModelMatrix());
        } else {
            float angleY = deltaX * camera->m_RotationSensitivity;
            float angleX = deltaY * camera->m_RotationSensitivity;
            for (auto cube : camera->rubiksCube.getSmallCubes()) {
                cube->setRotationMatrix(
                    glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0,1,0)) *
                    glm::rotate(glm::mat4(1.0f), glm::radians(angleX), glm::vec3(1,0,0)) *
                    cube->getRotationMatrix()
                );
            }
        }
    }
    // If right mouse button is pressed: either move the camera (if no cube is selected)
    // or translate the picked cube so that it stays under the mouse.
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (camera->rubiksCube.pickingMode && camera->rubiksCube.selectedCube) {
            // Get the viewport
            int viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            // Read depth at the current mouse coordinates.
            float winZ;
            glReadPixels((int)xpos, viewport[3] - (int)ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
            // Create a viewport vector
            glm::vec4 vp(0, 0, viewport[2], viewport[3]);
            // Unproject the window coordinates (x, y, winZ) into world coordinates.
            glm::vec3 winCoord(xpos, viewport[3] - ypos, winZ);
            glm::vec3 worldPos = glm::unProject(winCoord, camera->GetViewMatrix(), camera->GetProjectionMatrix(), vp);
            // Update the selected cube's model matrix to translate it to worldPos.
            // (You may wish to preserve the rotation; here we simply replace the translation.)
            glm::mat4 newModel = glm::translate(glm::mat4(1.0f), worldPos);
            camera->rubiksCube.selectedCube->setModelMatrix(newModel);
        } else {
            float sensitivity = 0.01f;
            camera->m_Position += glm::vec3(-deltaX * sensitivity, deltaY * sensitivity, 0);
            camera->UpdateViewMatrix();
        }
    }
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Camera* camera = (Camera*) glfwGetWindowUserPointer(window);
    if (!camera) return;
    float sensitivity = 0.5f;
    camera->m_Position += camera->m_Orientation * (float)yoffset * sensitivity;
    camera->UpdateViewMatrix();
}

void Camera::EnableInputs(GLFWwindow* window) {
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

// --- Key handling methods ---
void Camera::handleRKey() {
    std::cout << "R key pressed" << std::endl;
    if (rubiksCube.canRotateRightWall())
        rubiksCube.rotateRightWall();
}
void Camera::handleLKey() {
    std::cout << "L key pressed" << std::endl;
    if (rubiksCube.canRotateLeftWall())
        rubiksCube.rotateLeftWall();
}
void Camera::handleUKey() {
    std::cout << "U key pressed" << std::endl;
    if (rubiksCube.canRotateUpWall())
        rubiksCube.rotateUpWall();
}
void Camera::handleDKey() {
    std::cout << "D key pressed" << std::endl;
    if (rubiksCube.canRotateDownWall())
        rubiksCube.rotateDownWall();
}
void Camera::handleBKey() {
    std::cout << "B key pressed" << std::endl;
    if (rubiksCube.canRotateBackWall())
        rubiksCube.rotateBackWall();
}
void Camera::handleFKey() {
    std::cout << "F key pressed" << std::endl;
    if (rubiksCube.canRotateFrontWall())
        rubiksCube.rotateFrontWall();
}
void Camera::handleSpaceKey() {
    std::cout << "Space key pressed - Flip Rotation Direction" << std::endl;
    rubiksCube.RotationDirection = -rubiksCube.RotationDirection;
}
void Camera::handleZKey() {
    std::cout << "Z key pressed - Divide Rotation Angle by 2" << std::endl;
    rubiksCube.RotationAngle = std::max(rubiksCube.RotationAngle / 2, 45);
    std::cout << "New angle: " << rubiksCube.RotationDirection * rubiksCube.RotationAngle << std::endl;
}
void Camera::handleAKey() {
    std::cout << "A key pressed - Multiply Rotation Angle by 2" << std::endl;
    rubiksCube.RotationAngle = std::min(rubiksCube.RotationAngle * 2, 180);
    std::cout << "New angle: " << rubiksCube.RotationDirection * rubiksCube.RotationAngle << std::endl;
}
void Camera::handlePKey() {
    std::cout << "P key pressed - Toggle Picking Mode" << std::endl;
    rubiksCube.pickingMode = !rubiksCube.pickingMode;
    // When turning off picking mode, also clear any selected cube.
    if (!rubiksCube.pickingMode)
        rubiksCube.selectedCube = nullptr;
    std::cout << "Picking mode is now " << rubiksCube.pickingMode << std::endl;
}
void Camera::handleMKey() {
    std::cout << "M key pressed - Starting Mixer" << std::endl;
    std::ofstream mixerFile("mixer.txt");
    if (!mixerFile.is_open()) {
        std::cerr << "Error: Unable to open mixer.txt" << std::endl;
        return;
    }
    // Define valid actions. (Only perform moves if the corresponding canRotate function returns true.)
    std::vector<std::pair<char, std::string>> actions = {
        {'R', "Right wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + "°)"},
        {'L', "Left wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + "°)"},
        {'U', "Up wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + "°)"},
        {'D', "Down wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + "°)"},
        {'B', "Back wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + "°)"},
        {'F', "Front wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + "°)"}
    };
    int actionCount = 50 + (std::rand() % 50);
    for (int i = 0; i < actionCount; i++) {
        auto action = actions[std::rand() % actions.size()];
        switch(action.first) {
            case 'R': handleRKey(); break;
            case 'L': handleLKey(); break;
            case 'U': handleUKey(); break;
            case 'D': handleDKey(); break;
            case 'B': handleBKey(); break;
            case 'F': handleFKey(); break;
        }
        mixerFile << action.second << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    mixerFile.close();
    std::cout << "Mixer actions written to mixer.txt" << std::endl;
}
void Camera::handleUpArrow() {
    std::cout << "Up Arrow pressed" << std::endl;
    rubiksCube.UpArrow();
}
void Camera::handleDownArrow() {
    std::cout << "Down Arrow pressed" << std::endl;
    rubiksCube.DownArrow();
}
void Camera::handleLeftArrow() {
    std::cout << "Left Arrow pressed" << std::endl;
    rubiksCube.LeftArrow();
}
void Camera::handleRightArrow() {
    std::cout << "Right Arrow pressed" << std::endl;
    rubiksCube.RightArrow();
}
