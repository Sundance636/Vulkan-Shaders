#include "PhysicsSystem.h"


PhysicsSystem::PhysicsSystem(float gravity) : GravityStrength(gravity) {

}


//compute for each vector in vector field the net force an object
// enacts on it, and therefore its direction
void PhysicsSystem::ComputeField(std::vector<Entity>& vectorField, std::vector<Entity>& Objects) {
    
    for(int i = 0; i < vectorField.size(); i++) {
        glm::vec2 netVelocity{};

        for(int j = 0; j < Objects.size(); j++) {
            netVelocity += ComputeForce(vectorField[i],Objects[j]);
            

        }
    vectorField[i].transform2d.scale.x = 0.005f + 0.045f * glm::clamp(glm::log(glm::length(netVelocity) + 1) / 3.f, 0.f, 1.f);
        //apply net changes
        vectorField[i].transform2d.rotation = atan2(netVelocity.x,netVelocity.y);
    }

}

glm::vec2 PhysicsSystem::ComputeForce(Entity& vector, Entity& Object) {

    glm::vec2 distanceVec = vector.transform2d.translation - Object.transform2d.translation;

    float distanceMagnitudeSquared = glm::dot(distanceVec,distanceVec);


    return (GravityStrength * vector.bodyComponent.mass * Object.bodyComponent.mass * distanceVec)/distanceMagnitudeSquared;
}