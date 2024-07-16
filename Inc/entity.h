#ifndef __entity_h__
#define __entity_h__

#include "model.h"

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{};
    glm::vec3 rotation{};

    /*
    glm::mat2 mat2() { 
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);

        glm::mat2 rotMatrix{{c, s}, {-s, c}};

        glm::mat2 scaleMat{{scale.x, 0.0f},
                            {0.0f, scale.y}};

        return rotMatrix * scaleMat;
        }
    */

    glm::mat4 mat4() {
        auto transform = glm::scale(glm::mat4{1.0f}, scale);
        transform = glm::translate(transform, translation);

        transform = glm::rotate(transform, rotation.y, {0.0f,1.0f,0.0f});
        transform = glm::rotate(transform, rotation.x, {1.0f,0.0f,0.0f});
        transform = glm::rotate(transform, rotation.z, {0.0f,0.0f,1.0f});

        
        return transform;

    }

    glm::mat4 fastMat4() {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return glm::mat4{
        {
            scale.x * (c1 * c3 + s1 * s2 * s3),
            scale.x * (c2 * s3),
            scale.x * (c1 * s2 * s3 - c3 * s1),
            0.0f,
        },
        {
            scale.y * (c3 * s1 * s2 - c1 * s3),
            scale.y * (c2 * c3),
            scale.y * (c1 * c3 * s2 + s1 * s3),
            0.0f,
        },
        {
            scale.z * (c2 * s1),
            scale.z * (-s2),
            scale.z * (c1 * c2),
            0.0f,
        },
        {translation.x, translation.y, translation.z, 1.0f}};
  }
};


class Entity {

    public:
        using id_t = unsigned int;

        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        TransformComponent transform{};


        static Entity createEntity();
        id_t getId() {return id;}
        
        Entity(const Entity &) = delete;
        Entity &operator=(const Entity &) = delete;
        Entity(Entity &&) = default;
        Entity &operator=(Entity &&) = default;


    private:
        id_t id;

        Entity(id_t objectId);// : id{objectId} {}

};

#endif