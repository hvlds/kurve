#include "player_model.hpp"
#include "player_mesh.hpp"
#include "shader.hpp"
#include <iostream>

PlayerModel::PlayerModel(user_data_t* user_data){
    Shader shader("../shader/circle.vs", "../shader/circle.fs", user_data);
    this->shader_id = shader.get_id();

    PlayerMesh mesh(user_data);
    this->mesh = mesh;
}

void PlayerModel::update() {

}

void PlayerModel::init_uniforms() {
    
}