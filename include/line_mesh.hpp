#ifndef LINE_MESH_HPP
#define LINE_MESH_HPP

#include "mesh.hpp"
#include "user.hpp"

class LineMesh: public Mesh {
   public:
    LineMesh();
    virtual void draw() override;
};

#endif