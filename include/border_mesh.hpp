#ifndef BORDER_MESH_HPP
#define BORDER_MESH_HPP

#include "mesh.hpp"

class BorderMesh: public Mesh {
   public:
    BorderMesh();
    virtual void draw() override;
};

#endif