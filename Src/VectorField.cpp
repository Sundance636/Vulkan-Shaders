#include "VectorField.h"


VectorField VectorField::createVectorField() {
            static id_t currentId = 0;
            return VectorField{currentId++};
}

VectorField::VectorField(id_t objectId) : id{objectId} {
    
}

void VectorField::setPositon(float x, float y) {
    position = {x, y};
}

glm::vec2 VectorField::getPosition() {
    return position;
}

glm::mat3 VectorField::getTranslationMat() {
    return glm::mat3{{1.0f,0.0f,0.0f}, {0.0f, 1.0f, 0.0f},{position.x,position.y,1.0f}};
}
glm::mat3 getInvTranslationMat();
