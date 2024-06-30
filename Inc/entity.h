#ifndef __entity_h__
#define __entity_h__

#include "model.h"

#include <memory>

struct Transform2dComponent {
    glm::vec2 translation{};
    glm::vec2 scale{1.0f, 1.0f};
    float rotation;

    glm::mat2 mat2() { 
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);

        glm::mat2 rotMatrix{{c, s}, {-s, c}};

        glm::mat2 scaleMat{{scale.x, 0.0f},
                            {0.0f, scale.y}};

        return rotMatrix * scaleMat;
        }
};


class Entity {

    public:
        using id_t = unsigned int;

        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d;


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