#include "RubiksCube.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <Camera.h>

// A small tolerance for floating point comparisons.
const float epsilon = 0.0001f;

RubiksCube::RubiksCube()
        : RotationDirection(1), RotationAngle(90), Sensitivity(1.0f), pickingMode(false),
          locks{ false, false, false, false, false, false }, centerCube(nullptr), selectedCube(nullptr)
{
    generateSmallCubes(); // Automatically create the 27 small cubes.
}

RubiksCube::~RubiksCube() {
    for (SmallCube* cube : smallCubes)
        delete cube;
}

// Generate 27 small cubes arranged in a 3x3x3 grid.
// The center cube (index 13) is stored as centerCube.
void RubiksCube::generateSmallCubes() {
    smallCubes.clear();
    int index = 0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                glm::vec3 pos(x, y, z);
                SmallCube* cube = new SmallCube(pos, index);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                cube->setModelMatrix(model);
                smallCubes.push_back(cube);
                if (index == 13) {
                    centerCube = cube;
                }
                ++index;
            }
        }
    }
}

std::vector<SmallCube*> RubiksCube::getSmallCubes() {
    return smallCubes;
}

glm::vec3 RubiksCube::getPosition() {
    return centerCube->getPosition();
}

// Render function: first a visible pass then (if enabled) a picking pass.
void RubiksCube::render(Shader& shader, VertexArray& va, IndexBuffer& ib, glm::mat4 proj, glm::mat4 view, GLFWwindow* window) {
    // ---- Visible Rendering Pass ----
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (SmallCube* cube : smallCubes) {
        glm::mat4 model = cube->getRotationMatrix() * cube->getModelMatrix();
        glm::mat4 mvp = proj * view * model;

        shader.Bind();
        shader.SetPickingMode(false);
        glm::vec4 color(1.0f);
        shader.SetUniform4f("u_Color", color);

        shader.SetUniformMat4f("u_MVP", mvp);

        va.Bind();
        ib.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));

        shader.Unbind();
    }
    glfwSwapBuffers(window);

    // ---- Picking Pass (if enabled) ----
    if (pickingMode) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (SmallCube* cube : smallCubes) {
            glm::mat4 model = cube->getRotationMatrix() * cube->getModelMatrix();
            glm::mat4 mvp = proj * view * model;

            shader.Bind();
            // Each cube gets a unique color based on its index.
            glm::vec3 uniqueColor = glm::vec3(cube->index, cube->index, cube->index);
            glm::vec4 pickColor = glm::vec4(uniqueColor / 255.0f, 1.0f);
            shader.SetPickingMode(true);
            shader.SetUniform4f("u_Color", pickColor);
            shader.SetUniformMat4f("u_MVP", mvp);

            va.Bind();
            ib.Bind();
            GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
            shader.Unbind();
        }
        glFlush();
        glFinish();
    }
}

// ======================
// Rotation Functions
// ======================

// Rotate the RIGHT wall (cubes with x == center.x + 1) about the X-axis.
void RubiksCube::rotateRightWall() {
    std::cout << "Rotating Right Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();

    for (SmallCube* cube : smallCubes) {
        if (std::abs(cube->getPosition().x - (centerPos.x + 1)) < epsilon) {
            glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                                        glm::radians(static_cast<float>(RotationAngle * RotationDirection)),
                                        glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 back = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 finalTransform = back * rot * toOrigin;
            cube->setModelMatrix(finalTransform * cube->getModelMatrix());
        }
    }
    if (std::abs(RotationAngle) == 45)
        locks[0] = !locks[0];
}

// Rotate the LEFT wall (cubes with x == center.x - 1) about the X-axis.
void RubiksCube::rotateLeftWall() {
    std::cout << "Rotating Left Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();

    for (SmallCube* cube : smallCubes) {
        if (std::abs(cube->getPosition().x - (centerPos.x - 1)) < epsilon) {
            glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                                        glm::radians(static_cast<float>(RotationAngle * RotationDirection)),
                                        glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 back = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 finalTransform = back * rot * toOrigin;
            cube->setModelMatrix(finalTransform * cube->getModelMatrix());
        }
    }
    if (std::abs(RotationAngle) == 45)
        locks[1] = !locks[1];
}

// Rotate the UP wall (cubes with y == center.y + 1) about the Y-axis.
void RubiksCube::rotateUpWall() {
    std::cout << "Rotating Up Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();

    for (SmallCube* cube : smallCubes) {
        if (std::abs(cube->getPosition().y - (centerPos.y + 1)) < epsilon) {
            glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                                        glm::radians(static_cast<float>(RotationAngle * RotationDirection)),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 back = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 finalTransform = back * rot * toOrigin;
            cube->setModelMatrix(finalTransform * cube->getModelMatrix());
        }
    }
    if (std::abs(RotationAngle) == 45)
        locks[2] = !locks[2];
}

// Rotate the DOWN wall (cubes with y == center.y - 1) about the Y-axis.
void RubiksCube::rotateDownWall() {
    std::cout << "Rotating Down Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();

    for (SmallCube* cube : smallCubes) {
        if (std::abs(cube->getPosition().y - (centerPos.y - 1)) < epsilon) {
            glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                                        glm::radians(static_cast<float>(RotationAngle * RotationDirection)),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 back = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 finalTransform = back * rot * toOrigin;
            cube->setModelMatrix(finalTransform * cube->getModelMatrix());
        }
    }
    if (std::abs(RotationAngle) == 45)
        locks[3] = !locks[3];
}

// Rotate the BACK wall (cubes with z == center.z + 1) about the Z-axis.
void RubiksCube::rotateBackWall() {
    std::cout << "Rotating Back Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();

    for (SmallCube* cube : smallCubes) {
        if (std::abs(cube->getPosition().z - (centerPos.z + 1)) < epsilon) {
            glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                                        glm::radians(static_cast<float>(RotationAngle * RotationDirection)),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 back = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 finalTransform = back * rot * toOrigin;
            cube->setModelMatrix(finalTransform * cube->getModelMatrix());
        }
    }
    if (std::abs(RotationAngle) == 45)
        locks[4] = !locks[4];
}

// Rotate the FRONT wall (cubes with z == center.z - 1) about the Z-axis.
void RubiksCube::rotateFrontWall() {
    std::cout << "Rotating Front Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();

    for (SmallCube* cube : smallCubes) {
        if (std::abs(cube->getPosition().z - (centerPos.z - 1)) < epsilon) {
            glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                                        glm::radians(static_cast<float>(RotationAngle * RotationDirection)),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 back = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 finalTransform = back * rot * toOrigin;
            cube->setModelMatrix(finalTransform * cube->getModelMatrix());
        }
    }
    if (std::abs(RotationAngle) == 45)
        locks[5] = !locks[5];
}

// ======================
// Lock Checks for Wall Rotations
// ======================
bool RubiksCube::canRotateRightWall() {
    return !(locks[2] || locks[3] || locks[4] || locks[5]);
}

bool RubiksCube::canRotateLeftWall() {
    return !(locks[2] || locks[3] || locks[4] || locks[5]);
}

bool RubiksCube::canRotateUpWall() {
    return !(locks[0] || locks[1] || locks[4] || locks[5]);
}

bool RubiksCube::canRotateDownWall() {
    return !(locks[0] || locks[1] || locks[4] || locks[5]);
}

bool RubiksCube::canRotateBackWall() {
    return !(locks[0] || locks[1] || locks[2] || locks[3]);
}

bool RubiksCube::canRotateFrontWall() {
    return !(locks[0] || locks[1] || locks[2] || locks[3]);
}

// ======================
// Arrow Key Actions (Translation)
// ======================

// Moves all cubes upward (decreasing y) by Sensitivity units.
void RubiksCube::UpArrow() {
    for (SmallCube* cube : smallCubes) {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -Sensitivity, 0.0f));
        cube->setModelMatrix(translation * cube->getModelMatrix());
    }
}

// Moves all cubes downward (increasing y) by Sensitivity units.
void RubiksCube::DownArrow() {
    for (SmallCube* cube : smallCubes) {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, Sensitivity, 0.0f));
        cube->setModelMatrix(translation * cube->getModelMatrix());
    }
}

// Moves all cubes to the right (decreasing x) by Sensitivity units.
void RubiksCube::RightArrow() {
    for (SmallCube* cube : smallCubes) {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(-Sensitivity, 0.0f, 0.0f));
        cube->setModelMatrix(translation * cube->getModelMatrix());
    }
}

// Moves all cubes to the left (increasing x) by Sensitivity units.
void RubiksCube::LeftArrow() {
    for (SmallCube* cube : smallCubes) {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(Sensitivity, 0.0f, 0.0f));
        cube->setModelMatrix(translation * cube->getModelMatrix());
    }
}
