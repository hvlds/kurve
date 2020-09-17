#ifndef PLAYER_MESH_HPP
#define PLAYER_MESH_HPP

#include "mesh.hpp"
#include "user.hpp"

class PlayerMesh: public Mesh {
   public:
    PlayerMesh(user_data_t* user_data);
};

#endif