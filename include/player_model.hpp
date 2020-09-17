#ifndef PLAYER_MODEL_HPP
#define PLAYER_MODEL_HPP

#include "model.hpp"
#include "user.hpp"

class PlayerModel: public Model {
   public:
    PlayerModel(user_data_t* user_data);
    virtual void update() override;
};

#endif