#ifndef MODEL_HPP
#define MODEL_HPP

#include "mesh.hpp"
#include <vector>

class Model {
   private:
    std::vector<Mesh> meshes;
   public:
    Model();
    void draw();
};

#endif