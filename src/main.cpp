#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "display.hpp"
#include "game.hpp"
#include "gl_calls.hpp"
#include "user.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(void) {
    std::string json_path(STATIC_FILES);
    json_path.append("/settings/players.json");

    std::ifstream json_file(json_path);
    json players_json;
    json_file >> players_json;

    // Populate the information about the players from the 
    // file players.json
    std::vector<player_info_t> player_info;

    for (std::string player_name : players_json["players"]) {
        auto player_json = players_json[player_name];
        player_info_t player = {
            .is_active = false,
            .id = player_json["id"],
            .name = player_json["name"],
            .menu_text = player_json["menu_text"],
            .control = {
                player_json["control"][0], 
                player_json["control"][1]},
            .color = glm::vec3(
                player_json["color"][0],
                player_json["color"][1],
                player_json["color"][2]),
            .score = 0,
            .is_AI = false};
        
        player_info.push_back(player);
    }

    for (std::string AI_name : players_json["AI"]) {
        auto AI_json = players_json[AI_name];
        player_info_t AI = {
            .is_active = false,
            .id = AI_json["id"],
            .name = AI_json["name"],
            .menu_text = "--",
            .control = {
                0, 
                0},
            .color = glm::vec3(
                AI_json["color"][0],
                AI_json["color"][1],
                AI_json["color"][2]),
            .score = 0,
            .is_AI = true};
        
        player_info.push_back(AI);
    }

    // Create our user data struct:
    user_data_t user_data =
        {
            .window_width = 700,
            .window_height = 700,
            .is_fullscreen = false,
            .game_state = GAME_MENU,
            .delta_time = 0,
            .has_AI = false,
            .fps = 0,
            .player_info = &player_info,
            .grid = std::make_shared<Grid>()};

    Display display(&user_data);
    GLFWwindow* window = display.get_window();

    Game game(window);

    game.loop();

    game.terminate();

    display.terminate();

    return 0;
}
