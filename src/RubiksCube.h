#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include <vector>
#include "SmallCube.h"
#include <glm/glm.hpp>
#include <Shader.h>
#include <VertexArray.h>
#include <IndexBuffer.h>
#include <GLFW/glfw3.h>

class RubiksCube {
public:
    // Global rotation parameters.
    int RotationDirection; // 1 for clockwise, -1 for counterclockwise.
    int RotationAngle;     // Current rotation angle in degrees.
    float Sensitivity;     // Used for arrow key translations.
    bool pickingMode;      // Toggle for color picking mode.

    // Locks to prevent overlapping rotations on specific faces.
    bool locks[6];

    // Cube data.
    std::vector<SmallCube*> smallCubes;
    SmallCube* centerCube;
    SmallCube* selectedCube;

    // Constructor and Destructor.
    RubiksCube();
    ~RubiksCube();

    // Cube Generation & Rendering.
    void generateSmallCubes();
    void render(Shader& shader, VertexArray& va, IndexBuffer& ib, glm::mat4 proj, glm::mat4 view, GLFWwindow* window);

    // Face Rotations.
    void rotateRightWall();
    void rotateLeftWall();
    void rotateUpWall();
    void rotateDownWall();
    void rotateBackWall();
    void rotateFrontWall();

    // Checks if a particular face can be rotated.
    bool canRotateRightWall();
    bool canRotateLeftWall();
    bool canRotateUpWall();
    bool canRotateDownWall();
    bool canRotateBackWall();
    bool canRotateFrontWall();

    // Arrow key operations (translation of the entire cube).
    void UpArrow();
    void DownArrow();
    void LeftArrow();
    void RightArrow();

    // Getters.
    glm::vec3 getPosition();
    std::vector<SmallCube*> getSmallCubes();

private:
    // (Additional private members can be added here if needed)
};

#endif // RUBIKSCUBE_H
