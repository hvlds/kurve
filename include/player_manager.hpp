#ifndef PLAYER_MANAGER_HPP
#define PLAYER_MANAGER_HPP

#include "player_model.hpp"
#include <map>
#include <memory>

class PlayerManager {
   protected:
    std::map<int, std::shared_ptr<PlayerModel>> players;
   public:
    PlayerManager();
    void add_player();
};

#endif