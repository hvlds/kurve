#ifndef ARROW_MESH_HPP
#define ARROW_MESH_HPP

#include "mesh.hpp"
#include "user.hpp"

class ArrowMesh: public Mesh {
   private:
    glm::vec2 direction;   
   public:
    ArrowMesh(glm::vec3 color, glm::vec2 direction);
    ~ArrowMesh();
    virtual void draw() override;
};

#endif