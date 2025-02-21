#include "Camera.h"
#include <chrono>
#include <thread>
#include <algorithm>
#include <fstream>
#include <cstdlib>

//--------------------------------------------------
// Projection Setup
//--------------------------------------------------
void Camera::SetOrthographic(float nearPlane, float farPlane)
{
    m_Near = nearPlane;
    m_Far = farPlane;
    // Update the orthographic projection matrix
    m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, nearPlane, farPlane);
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}

void Camera::SetPerspective(float fov, float nearPlane, float farPlane)
{
    m_FOV = fov;
    m_PerspectiveNear = nearPlane;
    m_PerspectiveFar = farPlane;
    float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
    m_Projection = glm::perspective(glm::radians(m_FOV), aspectRatio, m_PerspectiveNear, m_PerspectiveFar);
    UpdateViewMatrix();
    std::cout << "Perspective set: FOV=" << m_FOV << ", Near=" << m_PerspectiveNear
              << ", Far=" << m_PerspectiveFar << ", AspectRatio=" << aspectRatio << std::endl;
}

void Camera::SetPosition(glm::vec3 newPosition)
{
    m_Position = newPosition;
    UpdateViewMatrix();
}

//--------------------------------------------------
// Input Callback Functions (using improved signatures)
//--------------------------------------------------

// Key input callback (no changes to signature)
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (!cam) {
        std::cout << "Warning: Camera not set as Window User Pointer! Skipping KeyCallback." << std::endl;
        return;
    }
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_UP:    cam->handleUpArrow(); break;
            case GLFW_KEY_DOWN:  cam->handleDownArrow(); break;
            case GLFW_KEY_LEFT:  cam->handleLeftArrow(); break;
            case GLFW_KEY_RIGHT: cam->handleRightArrow(); break;
            case GLFW_KEY_R:     cam->handleRKey(); break;
            case GLFW_KEY_L:     cam->handleLKey(); break;
            case GLFW_KEY_U:     cam->handleUKey(); break;
            case GLFW_KEY_D:     cam->handleDKey(); break;
            case GLFW_KEY_B:     cam->handleBKey(); break;
            case GLFW_KEY_F:     cam->handleFKey(); break;
            case GLFW_KEY_SPACE: cam->handleSpaceKey(); break;
            case GLFW_KEY_Z:     cam->handleZKey(); break;
            case GLFW_KEY_A:     cam->handleAKey(); break;
            case GLFW_KEY_P:     cam->handlePKey(); break;
            case GLFW_KEY_M:
                std::thread([cam]() { cam->handleMKey(); }).detach();
                break;
            default:
                break;
        }
    }
}

// Corrected mouse button callback signature
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (!cam) {
        std::cout << "Warning: Camera not set as Window User Pointer! Skipping MouseButtonCallback." << std::endl;
        return;
    }

    // Report left-button clicks
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        std::cout << "Mouse LEFT button pressed." << std::endl;
    }

    // Right mouse button for picking
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        std::cout << "Mouse RIGHT button pressed." << std::endl;
        if (cam->rubiksCube.pickingMode)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            int flippedY = cam->m_Height - static_cast<int>(mouseY);

            unsigned char pickedColor[4] = {0, 0, 0, 0};
            glReadPixels(static_cast<int>(mouseX), flippedY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pickedColor);

            int colorID = static_cast<int>(pickedColor[0]);
            int shapeID = colorID; // decoding strategy

            if (shapeID < 0 || shapeID > 26)
            {
                cam->rubiksCube.selectedCube = nullptr;
            }
            else
            {
                for (SmallCube* cube : cam->rubiksCube.smallCubes)
                {
                    if (cube->index == shapeID)
                    {
                        cam->rubiksCube.selectedCube = cube;
                        std::cout << "Selected cube index: " << cube->index << std::endl;
                        break;
                    }
                }
            }
            std::cout << "Picked Color: [R: " << static_cast<int>(pickedColor[0])
                      << ", G: " << static_cast<int>(pickedColor[1])
                      << ", B: " << static_cast<int>(pickedColor[2]) << "]" << std::endl;
            std::cout << "Decoded Shape ID: " << shapeID << std::endl;
        }
    }
}

// Cursor position callback handles both cube rotation and camera movement.
void CursorPosCallback(GLFWwindow* window, double currX, double currY)
{
    auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (!cam) {
        std::cout << "Warning: Camera not set as Window User Pointer! Skipping CursorPosCallback." << std::endl;
        return;
    }

    float deltaX = static_cast<float>(cam->m_OldMouseX - currX);
    float deltaY = static_cast<float>(cam->m_OldMouseY - currY);
    cam->m_OldMouseX = currX;
    cam->m_OldMouseY = currY;

    // Left button: rotate either a selected cube or all cubes.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (cam->rubiksCube.pickingMode)
        {
            SmallCube* cube = cam->rubiksCube.selectedCube;
            if (cube)
            {
                float rotAngleY = deltaX * cam->m_RotationSensitivity;
                float rotAngleX = deltaY * cam->m_RotationSensitivity;
                glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), -cube->getPosition());
                glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(rotAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
                glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(rotAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), cube->getPosition());
                glm::mat4 finalTransform = translateBack * rotY * rotX * translateToOrigin;
                cube->setModelMatrix(finalTransform * cube->getModelMatrix());
            }
        }
        else
        {
            float rotAngleY = deltaX * cam->m_RotationSensitivity;
            float rotAngleX = glm::clamp(deltaY * cam->m_RotationSensitivity, -89.0f, 89.0f);
            glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(rotAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(rotAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 combinedRotation = rotY * rotX;
            for (SmallCube* cube : cam->rubiksCube.getSmallCubes())
            {
                cube->setRotationMatrix(combinedRotation * cube->getRotationMatrix());
            }
        }
    }
        // Right button: translate either the selected cube or the camera.
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (cam->rubiksCube.pickingMode)
        {
            SmallCube* cube = cam->rubiksCube.selectedCube;
            if (cube)
            {
                float sensitivity = 0.02f * cam->m_Position.z / 15.0f;
                glm::vec3 translation(deltaX * sensitivity, -deltaY * sensitivity, 0.0f);
                glm::mat4 transMat = glm::translate(glm::mat4(1.0f), translation);
                cube->setModelMatrix(transMat * cube->getModelMatrix());
            }
        }
        else
        {
            float sensitivity = 0.005f;
            cam->m_Position += glm::normalize(glm::cross(cam->m_Orientation, cam->m_Up)) * deltaX * sensitivity;
            cam->m_Position += cam->m_Up * deltaY * sensitivity;
            cam->UpdateViewMatrix();
        }
    }
}

// Scroll callback: zoom the camera along its orientation.
void ScrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
    auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (!cam) {
        std::cout << "Warning: Camera not set as Window User Pointer! Skipping ScrollCallback." << std::endl;
        return;
    }
    float sensitivity = 0.5f;
    cam->m_Position += cam->m_Orientation * static_cast<float>(offsetY) * sensitivity;
    cam->UpdateViewMatrix();
    std::cout << "Scroll motion processed." << std::endl;
}

//--------------------------------------------------
// Camera Input Enabling
//--------------------------------------------------
void Camera::EnableInputs(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

//--------------------------------------------------
// View Matrix Update
//--------------------------------------------------
void Camera::UpdateViewMatrix()
{
    // Compute the right vector correctly using the camera's orientation and up vector.
    glm::vec3 right = glm::normalize(glm::cross(m_Orientation, m_Up));
    glm::vec3 recalculatedUp = glm::normalize(glm::cross(right, m_Orientation));

    // Apply pitch and yaw rotations to the orientation.
    m_Orientation = glm::rotate(m_Orientation, glm::radians(m_RotationAngleX), right);
    m_Orientation = glm::rotate(m_Orientation, glm::radians(m_RotationAngleY), m_Up);

    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, recalculatedUp);
}

//--------------------------------------------------
// Rubik's Cube Interaction Key Handlers
//--------------------------------------------------
void Camera::handleRKey()
{
    std::cout << "R key pressed." << std::endl;
    if (rubiksCube.canRotateRightWall())
        rubiksCube.rotateRightWall();
}

void Camera::handleLKey()
{
    std::cout << "L key pressed." << std::endl;
    if (rubiksCube.canRotateLeftWall())
        rubiksCube.rotateLeftWall();
}

void Camera::handleUKey()
{
    std::cout << "U key pressed." << std::endl;
    if (rubiksCube.canRotateUpWall())
        rubiksCube.rotateUpWall();
}

void Camera::handleDKey()
{
    std::cout << "D key pressed." << std::endl;
    if (rubiksCube.canRotateDownWall())
        rubiksCube.rotateDownWall();
}

void Camera::handleBKey()
{
    std::cout << "B key pressed." << std::endl;
    if (rubiksCube.canRotateBackWall())
        rubiksCube.rotateBackWall();
}

void Camera::handleFKey()
{
    std::cout << "F key pressed." << std::endl;
    if (rubiksCube.canRotateFrontWall())
        rubiksCube.rotateFrontWall();
}

void Camera::handleSpaceKey()
{
    std::cout << "Space key pressed - flipping rotation direction." << std::endl;
    rubiksCube.RotationDirection = -rubiksCube.RotationDirection;
}

void Camera::handleZKey()
{
    std::cout << "Z key pressed - halving rotation angle." << std::endl;
    // Ensure the value is kept as a float.
    rubiksCube.RotationAngle = std::max(rubiksCube.RotationAngle / 2.0f, 45.0f);
    std::cout << "New rotation angle: " << rubiksCube.RotationAngle * rubiksCube.RotationDirection << std::endl;
}

void Camera::handleAKey()
{
    std::cout << "A key pressed - doubling rotation angle." << std::endl;
    rubiksCube.RotationAngle = std::min(rubiksCube.RotationAngle * 2.0f, 180.0f);
    std::cout << "New rotation angle: " << rubiksCube.RotationAngle * rubiksCube.RotationDirection << std::endl;
}

//--------------------------------------------------
// Mixer (Bonus) - Randomized Rubik's Cube Moves
//--------------------------------------------------
void Camera::handleMKey() {
    std::cout << "M key pressed - the Mixer will start the work..." << std::endl;
    std::ofstream mixerFile("mixer.txt");
    if (!mixerFile.is_open()) {
        std::cerr << "Error: Unable to open mixer.txt for writing." << std::endl;
        return;
    }
    std::cout << "File mixer.txt opened successfully." << std::endl;

    // Use the instance 'rubiksCube' to access non-static members.
    std::vector<std::pair<char, std::string>> actions = {
            {'R', "Right wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + " deg)"},
            {'L', "Left wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + " deg)"},
            {'U', "Up wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + " deg)"},
            {'D', "Down wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + " deg)"},
            {'B', "Back wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + " deg)"},
            {'F', "Front wall rotation (" + std::to_string(rubiksCube.RotationDirection * rubiksCube.RotationAngle) + " deg)"}
    };

    int actionCount = 50 + (std::rand() % 50);
    std::cout << "Number of actions to perform: " << actionCount << std::endl;

    for (int i = 0; i < actionCount; ++i) {
        const auto& action = actions[std::rand() % actions.size()];
        switch (action.first) {
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
    std::cout << "Mixer actions have been written to mixer.txt." << std::endl;
}

void Camera::handlePKey()
{
    std::cout << "P key pressed - toggling picking mode." << std::endl;
    rubiksCube.pickingMode = !rubiksCube.pickingMode;
    std::cout << "Picking mode is now: " << rubiksCube.pickingMode << std::endl;
}

//--------------------------------------------------
// Arrow Key Handlers for Rubik's Cube Movement
//--------------------------------------------------
void Camera::handleUpArrow()
{
    std::cout << "UP Arrow pressed." << std::endl;
    rubiksCube.UpArrow();
}

void Camera::handleDownArrow()
{
    std::cout << "Down Arrow pressed." << std::endl;
    rubiksCube.DownArrow();
}

void Camera::handleLeftArrow()
{
    std::cout << "Left Arrow pressed." << std::endl;
    rubiksCube.LeftArrow();
}

void Camera::handleRightArrow()
{
    std::cout << "Right Arrow pressed." << std::endl;
    rubiksCube.RightArrow();
}
