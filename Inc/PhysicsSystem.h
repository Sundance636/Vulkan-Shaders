#ifndef __PhysicsSystem_h__
#define __PhysicsSystem_h__

#include "entity.h"

class PhysicsSystem {

//compute for each vector in vector field the net force and therefore direction
    private:
        float GravityStrength;


    public:

        PhysicsSystem(float gravity);
        void ComputeObjects();
        void ComputeField(std::vector<Entity>& vectorField, std::vector<Entity>& Objects);
        glm::vec2 ComputeForce(Entity& vector, Entity& Object);

};


#endif