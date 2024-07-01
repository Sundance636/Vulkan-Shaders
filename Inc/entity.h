#ifndef __entity_h__
#define __entity_h__

#include "model.h"

#include <memory>


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