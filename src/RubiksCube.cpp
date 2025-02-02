#include "RubiksCube.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>

const float epsilon = 0.01f;

RubiksCube::RubiksCube()
    : RotationDirection(1), RotationAngle(90), Sensitivity(0.1f), pickingMode(false), selectedCube(nullptr)
{
    for (int i = 0; i < 6; i++) locks[i] = false;
    generateSmallCubes();
}

RubiksCube::~RubiksCube() {
    for (auto cube : smallCubes)
        delete cube;
}

void RubiksCube::generateSmallCubes() {
    smallCubes.clear();
    int index = 0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                glm::vec3 pos(x, y, z);
                SmallCube* cube = new SmallCube(pos, index);
                cube->setModelMatrix(glm::translate(glm::mat4(1.0f), pos));
                cube->setRotationMatrix(glm::mat4(1.0f));
                smallCubes.push_back(cube);
                if (index == 13)
                    centerCube = cube;
                index++;
            }
        }
    }
}

void RubiksCube::render(Shader &shader, VertexArray &va, IndexBuffer &ib, glm::mat4 proj, glm::mat4 view, GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto cube : smallCubes) {
        glm::mat4 model = cube->getRotationMatrix() * cube->getModelMatrix();
        glm::mat4 mvp = proj * view * model;
        shader.Bind();
        if (pickingMode) {
            int id = cube->index;
            float r = ((id + 1) & 0xFF) / 255.0f;
            float g = (((id + 1) >> 8) & 0xFF) / 255.0f;
            float b = (((id + 1) >> 16) & 0xFF) / 255.0f;
            glm::vec4 pickingColor(r, g, b, 1.0f);
            shader.SetUniform4f("u_Color", pickingColor);
            shader.SetUniform1i("u_Texture", 1);
        } else {
            glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
            shader.SetUniform4f("u_Color", color);
            shader.SetUniform1i("u_Texture", 0);
        }
        shader.SetUniformMat4f("u_MVP", mvp);
        va.Bind();
        ib.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
        shader.Unbind();
    }
    glfwSwapBuffers(window);
}

void RubiksCube::rotateRightWall() {
    std::cout << "Rotating Right Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();
    for (auto cube : smallCubes) {
        if (std::abs(cube->getPosition().x - (centerPos.x - 1)) < epsilon) {
            glm::mat4 transToOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                glm::radians((float)RotationAngle * RotationDirection), glm::vec3(1, 0, 0));
            glm::mat4 transBack = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 final = transBack * rot * transToOrigin;
            cube->setModelMatrix(final * cube->getModelMatrix());
        }
    }
}

void RubiksCube::rotateLeftWall() {
    std::cout << "Rotating Left Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();
    for (auto cube : smallCubes) {
        if (std::abs(cube->getPosition().x - (centerPos.x + 1)) < epsilon) {
            glm::mat4 transToOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                glm::radians((float)RotationAngle * RotationDirection), glm::vec3(1, 0, 0));
            glm::mat4 transBack = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 final = transBack * rot * transToOrigin;
            cube->setModelMatrix(final * cube->getModelMatrix());
        }
    }
}

void RubiksCube::rotateUpWall() {
    std::cout << "Rotating Up Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();
    for (auto cube : smallCubes) {
        if (std::abs(cube->getPosition().z - (centerPos.z + 1)) < epsilon) {
            glm::mat4 transToOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                glm::radians((float)RotationAngle * RotationDirection), glm::vec3(0, 0, 1));
            glm::mat4 transBack = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 final = transBack * rot * transToOrigin;
            cube->setModelMatrix(final * cube->getModelMatrix());
        }
    }
}

void RubiksCube::rotateDownWall() {
    std::cout << "Rotating Down Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();
    for (auto cube : smallCubes) {
        if (std::abs(cube->getPosition().z - (centerPos.z - 1)) < epsilon) {
            glm::mat4 transToOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                glm::radians((float)RotationAngle * RotationDirection), glm::vec3(0, 0, 1));
            glm::mat4 transBack = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 final = transBack * rot * transToOrigin;
            cube->setModelMatrix(final * cube->getModelMatrix());
        }
    }
}

void RubiksCube::rotateBackWall() {
    std::cout << "Rotating Back Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();
    for (auto cube : smallCubes) {
        if (std::abs(cube->getPosition().y - (centerPos.y - 1)) < epsilon) {
            glm::mat4 transToOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                glm::radians((float)RotationAngle * RotationDirection), glm::vec3(0, 1, 0));
            glm::mat4 transBack = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 final = transBack * rot * transToOrigin;
            cube->setModelMatrix(final * cube->getModelMatrix());
        }
    }
}

void RubiksCube::rotateFrontWall() {
    std::cout << "Rotating Front Wall by " << (RotationAngle * RotationDirection) << " degrees\n";
    glm::vec3 centerPos = centerCube->getPosition();
    for (auto cube : smallCubes) {
        if (std::abs(cube->getPosition().y - (centerPos.y + 1)) < epsilon) {
            glm::mat4 transToOrigin = glm::translate(glm::mat4(1.0f), -centerPos);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f),
                glm::radians((float)RotationAngle * RotationDirection), glm::vec3(0, 1, 0));
            glm::mat4 transBack = glm::translate(glm::mat4(1.0f), centerPos);
            glm::mat4 final = transBack * rot * transToOrigin;
            cube->setModelMatrix(final * cube->getModelMatrix());
        }
    }
}

void RubiksCube::UpArrow() {
    glm::vec3 t(0, Sensitivity, 0);
    for (auto cube : smallCubes)
        cube->setModelMatrix(glm::translate(glm::mat4(1.0f), t) * cube->getModelMatrix());
}
void RubiksCube::DownArrow() {
    glm::vec3 t(0, -Sensitivity, 0);
    for (auto cube : smallCubes)
        cube->setModelMatrix(glm::translate(glm::mat4(1.0f), t) * cube->getModelMatrix());
}
void RubiksCube::RightArrow() {
    glm::vec3 t(-Sensitivity, 0, 0);
    for (auto cube : smallCubes)
        cube->setModelMatrix(glm::translate(glm::mat4(1.0f), t) * cube->getModelMatrix());
}
void RubiksCube::LeftArrow() {
    glm::vec3 t(Sensitivity, 0, 0);
    for (auto cube : smallCubes)
        cube->setModelMatrix(glm::translate(glm::mat4(1.0f), t) * cube->getModelMatrix());
}

glm::vec3 RubiksCube::getPosition() {
    return centerCube->getPosition();
}
