#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include <GLFW/glfw3.h>

// ----- Minimal SmallCube class (if you already have one, remove this) -----
class SmallCube {
public:
    int index;                    // Unique index [0,26]
    glm::mat4 modelMatrix;        // Local translation matrix
    glm::mat4 rotationMatrix;     // Accumulated rotation matrix
    glm::vec3 position;           // Logical center position

    SmallCube(glm::vec3 pos, int idx)
        : index(idx), position(pos) {
        modelMatrix = glm::translate(glm::mat4(1.0f), pos);
        rotationMatrix = glm::mat4(1.0f);
    }
    glm::vec3 getPosition() const { return position; }
    glm::mat4 getModelMatrix() const { return modelMatrix; }
    void setModelMatrix(const glm::mat4 &m) { modelMatrix = m; }
    glm::mat4 getRotationMatrix() const { return rotationMatrix; }
    void setRotationMatrix(const glm::mat4 &m) { rotationMatrix = m; }
};
// ----- End SmallCube class -----

class RubiksCube {
public:
    // Rotation parameters (in degrees)
    int RotationDirection;   // 1 for clockwise, -1 for counterclockwise
    int RotationAngle;       // Default 90
    float Sensitivity;       // For arrow key translations (e.g. 0.1f)
    bool pickingMode;        // Toggled by 'P'

    // Minimal locks (if you wish to add restrictions)
    bool locks[6]; // [0]: right, [1]: left, [2]: up, [3]: down, [4]: back, [5]: front

    // The Rubik’s Cube is composed of small cubes.
    std::vector<SmallCube*> smallCubes;
    SmallCube* centerCube;   // For example, cube with index 13
    SmallCube* selectedCube; // When a cube is picked

    // Constructor and destructor
    RubiksCube();
    ~RubiksCube();

    // Generate the 27 small cubes
    void generateSmallCubes();

    // Render all small cubes.
    // When pickingMode is true, each cube is drawn with a unique color.
    void render(Shader &shader, VertexArray &va, IndexBuffer &ib, glm::mat4 proj, glm::mat4 view, GLFWwindow* window);

    // Face rotations – each rotates the corresponding wall by (RotationAngle * RotationDirection) degrees.
    void rotateRightWall();
    void rotateLeftWall();
    void rotateUpWall();
    void rotateDownWall();
    void rotateBackWall();
    void rotateFrontWall();

    // Minimal lock query functions (always return true for now)
    bool canRotateRightWall() { return true; }
    bool canRotateLeftWall()  { return true; }
    bool canRotateUpWall()    { return true; }
    bool canRotateDownWall()  { return true; }
    bool canRotateBackWall()  { return true; }
    bool canRotateFrontWall() { return true; }

    // Arrow key operations: translate every cube by a small vector.
    void UpArrow();
    void DownArrow();
    void LeftArrow();
    void RightArrow();

    // Get the center position (from centerCube)
    glm::vec3 getPosition();

    // Return the vector of small cubes.
    std::vector<SmallCube*> getSmallCubes() { return smallCubes; }
};

#endif // RUBIKSCUBE_H
