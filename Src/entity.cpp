#include "entity.h"


Entity Entity::createEntity() {
            static id_t currentId = 0;
            return Entity{currentId++};
}

Entity::Entity(id_t objectId) : id{objectId} {
    
}