#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <map>
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
            .color = {0xFF, 0x00, 0x00},
            .menu_color = glm::vec3(
                player_json["menu_color"][0],
                player_json["menu_color"][1],
                player_json["menu_color"][2]),
            .score = 0};
    }

    // Define the basic information for the six players
    // The Information is hardcoded fixed and cannot be edited!
    player_info_t player_1 = {
        .is_active = false,
        .id = 1,
        .name = "Gryffindor",
        .menu_text = "Gryffindor   <L.Ctrl L.Alt>",
        .control = {GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_ALT},
        .color = {0xFF, 0x00, 0x00},
        .menu_color = glm::vec3(1.0f, 0.0f, 0.0f),
        .score = 0};

    player_info_t player_2 = {
        .is_active = false,
        .id = 2,
        .name = "Slytherin",
        .menu_text = "Slytherin    <1 Q>",
        .control = {GLFW_KEY_1, GLFW_KEY_Q},
        .color = {0x00, 0xFF, 0x00},
        .menu_color = glm::vec3(0.0f, 1.0f, 0.0f),
        .score = 0};

    player_info_t player_3 = {
        .is_active = false,
        .id = 3,
        .name = "Hufflepuff",
        .menu_text = "Hufflepuff   <M ,>",
        .control = {GLFW_KEY_M, GLFW_KEY_COMMA},
        .color = {0xFF, 0xFF, 0x00},
        .menu_color = glm::vec3(1.0f, 1.0f, 0.0f),
        .score = 0};

    player_info_t player_4 = {
        .is_active = false,
        .id = 4,
        .name = "Ravenclaw",
        .menu_text = "Ravenclaw    <L.Arrow R.Arrow>",
        .control = {GLFW_KEY_LEFT, GLFW_KEY_RIGHT},
        .color = {0x00, 0x66, 0xFF},
        .menu_color = glm::vec3(0.0f, 0.5f, 1.0f),
        .score = 0};

    player_info_t player_5 = {
        .is_active = false,
        .id = 5,
        .name = "Muggle",
        .menu_text = "Muggle       <O P>",
        .control = {GLFW_KEY_O, GLFW_KEY_P},
        .color = {0xDD, 0x00, 0xDD},
        .menu_color = glm::vec3(1.0f, 0.11f, 0.68f),
        .score = 0};

    player_info_t player_6 = {
        .is_active = false,
        .id = 6,
        .name = "Squib",
        .menu_text = "Squib        <B N>",
        .control = {GLFW_KEY_B, GLFW_KEY_N},
        .color = {0xDD, 0xDD, 0xDD},
        .menu_color = glm::vec3(0.7f, 0.7f, 0.7f),
        .score = 0};

    std::vector<player_info_t> player_info = {
        player_1,
        player_2,
        player_3,
        player_4,
        player_5,
        player_6};

    // Create our user data struct:
    user_data_t user_data =
        {
            .window_width = 700,
            .window_height = 700,
            .game_state = GAME_MENU,
            .player_info = &player_info};

    Display display(&user_data);
    GLFWwindow* window = display.get_window();

    Game game(window);

    game.loop();

    game.terminate();

    display.terminate();

    return 0;
}
