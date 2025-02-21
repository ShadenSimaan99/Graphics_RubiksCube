#ifndef SMALLCUBE_H
#define SMALLCUBE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

class SmallCube {

public:
    // Model matrix for transformations
    int index ;


    // Constructor
    SmallCube(const glm::vec3& pos , int index);
    SmallCube() ;

    // Getters
    glm::vec3 getPosition() const;
    glm::mat4 getModelMatrix() const;   // Getter for the model matrix
    glm::mat4 getRotationMatrix() const;   // Getter for the model matrix




    // Setters
    void setModelMatrix(const glm::mat4& matrix); // Setter for the model matrix
    void setRotationMatrix(const glm::mat4& matrix); // Setter for the model matrix



private :
    glm::mat4 modelMatrix;
    glm::mat4 RotationMatrix  ;



};


#endif // SMALLCUBE_H

