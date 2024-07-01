#include "VectorField.h"


VectorField VectorField::createVectorField() {
            static id_t currentId = 0;
            return VectorField{currentId++};
}

VectorField::VectorField(id_t objectId) : id{objectId} {
    
}