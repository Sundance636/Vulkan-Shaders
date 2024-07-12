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
        
        float H = glm::clamp(glm::log(glm::length(netVelocity) + 1) / (GravityStrength * 3), 0.f, 1.f);
        float R = fabs(H * 6.0f - 3.0f) - 1.0f;
        float G = 2.0f - fabs(H * 6.0f - 2.0f);
        float B = 2.0f - fabs(H * 6.0f - 4.0f);  

glm::vec3 newcolor( 
    std::max(0.0f, std::min(1.0f, R)),
    std::max(0.0f, std::min(1.0f, G)),
    std::max(0.0f, std::min(1.0f, B)));

        vectorField[i].color = newcolor;
    vectorField[i].transform2d.scale.x = 0.005f + 0.045f * glm::clamp(glm::log(glm::length(netVelocity) + 1) / 3.f, 0.f, 1.f);
        //apply net changes
        vectorField[i].transform2d.rotation = atan2(netVelocity.x,netVelocity.y);
    }

}

glm::vec2 PhysicsSystem::ComputeForce(Entity& vector, Entity& Object) {

    glm::vec2 distanceVec = vector.transform2d.translation - Object.transform2d.translation;

    float distanceMagnitudeSquared = glm::dot(distanceVec,distanceVec);

    //avoid aproaching division by zero and objects spontatneously repelling    
    if (glm::abs(distanceMagnitudeSquared) < 1e-10f) {
        return {.0f, .0f};
        }

    return (GravityStrength * vector.bodyComponent.mass * Object.bodyComponent.mass * distanceVec)/distanceMagnitudeSquared;
}

void PhysicsSystem::ComputeObjects(std::vector<Entity>& physicsObjs, float dt, unsigned int substeps =1) {
    float stepDelta = dt / substeps;
    for (int i = 0; i < substeps; i++) {

      for (auto iterA = physicsObjs.begin(); iterA != physicsObjs.end(); ++iterA) {
      auto& objA = *iterA;
      for (auto iterB = iterA; iterB != physicsObjs.end(); ++iterB) {
        if (iterA == iterB) continue;
        auto& objB = *iterB;

        auto force = ComputeForce(objA, objB);
        objA.bodyComponent.velocity += dt * -force / objA.bodyComponent.mass;
        objB.bodyComponent.velocity += dt * force / objB.bodyComponent.mass;
      }
    }

    // update each objects position based on its final velocity
    for (auto& obj : physicsObjs) {
      obj.transform2d.translation += dt * obj.bodyComponent.velocity;
    }
    }

}
