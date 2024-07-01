#ifndef __VectorField_h__
#define __VectorField_h__

#include "model.h"

#include <memory>

class VectorField {

    public:
        using id_t = unsigned int;

        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d;
        glm::vec2 position;
        glm::vec2 direction;

        void setPositon(float x, float y);
        void setDirection();
        glm::vec2 getPosition();

        glm::mat3 getTranslationMat();
        glm::mat3 getInvTranslationMat();

        static VectorField createVectorField();
        id_t getId() {return id;}
        
        VectorField(const VectorField &) = delete;
        VectorField &operator=(const VectorField &) = delete;
        VectorField(VectorField &&) = default;
        VectorField &operator=(VectorField &&) = default;


    private:
        id_t id;

        VectorField(id_t objectId);// : id{objectId} {}

};

#endif