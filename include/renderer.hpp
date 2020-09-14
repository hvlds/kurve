#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "mesh.hpp"
#include <vector>

class Renderer {
   private:
    std::vector<Mesh> meshes;
   public:
    Renderer();
    void draw();
};

#endif