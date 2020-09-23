#include "player_model.hpp"

#include <iostream>

#include "gl_calls.hpp"
#include "player_mesh.hpp"
#include "shader.hpp"

PlayerModel::PlayerModel(
    int id, GLfloat x, GLfloat y, std::array<GLubyte, 3> color) {       
    std::cout << "---- INIT PLAYER ----" << std::endl;
    this->model_type = MODEL_PLAYER;
    this->color = color;
    this->id = id;
    this->is_active = true;
    Point point{x, y};
    this->last_point = point;

    // Compile and add the shaders
    Shader shader("../shader/player.vs", "../shader/player.fs", &this->shader_id);
    // Init the uniforms
    this->init_uniforms();

    // Add the mesh
    auto mesh = std::make_shared<PlayerMesh>();
    this->mesh = mesh;

    // Init the values of the model
    this->start_pos_x = x;
    this->start_pos_y = y;
    this->init_values();    

    // Add initial positions into the point vector
    this->points.push_back(point);

    // Create a line model for the player
    auto line = std::make_shared<LineModel>(point, this->color);
    this->line_model = line;
}

void PlayerModel::draw() {
    glUseProgram(this->shader_id);
    this->mesh->draw();

    if (this->line_model != nullptr) {
        line_model->draw();
    }
}    

void PlayerModel::update(GLFWwindow* window) {
    // Update the time and calculate the delta:
    user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);
    GameState game_state = user_data->game_state;
    if (game_state == GAME_PAUSE) {
        double new_time = glfwGetTime();
        this->time = new_time;
    } else if (game_state == GAME_ACTIVE) {
        if (this->is_active == true) {
            glUseProgram(this->shader_id);
            double new_time = glfwGetTime();
            double time_delta = new_time - this->time;
            double speed = 2.5; 

            if (this->trans_x + this->start_pos_x >= 13.5 
                || this->trans_x + this->start_pos_x <= -18.5) {
                this->is_active = false;
                return;
            }

            if (this->trans_y + this->start_pos_y >= 18.5 
                || this->trans_y + this->start_pos_y <= -18.5) {
                this->is_active = false;
                return;
            }

            this->time = new_time;
            GLfloat angle_diff = 0;

            int right_state = glfwGetKey(window, this->control.right_key);
            if (right_state == GLFW_PRESS) {
                angle_diff = static_cast<GLfloat>((-speed * time_delta));        
            }

            int left_state = glfwGetKey(window, this->control.left_key);
            if (left_state == GLFW_PRESS) {
                angle_diff = static_cast<GLfloat>((speed * time_delta));
            }

            // std::cout << "Angle diff: " << angle_diff << std::endl;

            Vector speed_vec{this->speed_x, this->speed_y};

            if (angle_diff != 0) {
                GLfloat temp_speed_x = this->speed_x;
                GLfloat temp_speed_y = this->speed_y;

                this->speed_x = temp_speed_x * cos(angle_diff) - temp_speed_y * sin(angle_diff);
                this->speed_y = temp_speed_x * sin(angle_diff) + temp_speed_y * cos(angle_diff);
            }

            // std::cout << "Speed length: " << speed_vec.get_length() << std::endl;
            
            this->trans_x += this->speed_x;
            this->trans_y += this->speed_y;    

            Point last_point = this->points.back();
            Point point{
                this->trans_x + this->start_pos_x,
                this->trans_y + this->start_pos_y
            };
            this->points.push_back(point);
            if (this->line_model != nullptr) {          
                this->line_model->add_point(point);
                this->line_model->update(window); 
            }

            // Update the uniform:
            glUniform1f(this->trans_y_loc, this->trans_y);
            gl_check_error("glUniform1f [trans_y]");

            glUniform1f(this->trans_x_loc, this->trans_x);
            gl_check_error("glUniform1f [trans_x]");
        }
    }
}

void PlayerModel::init_uniforms() {
    std::cout << "Init uniforms" << std::endl;
    // Y trans:
    this->trans_y_loc = glGetUniformLocation(this->shader_id, "trans_y");
    gl_check_error("glGetUniformLocation [trans_y]");
    check_error(this->trans_y_loc >= 0, "Failed to obtain uniform location for trans_y.");

    // X trans:
    this->trans_x_loc = glGetUniformLocation(this->shader_id, "trans_x");
    gl_check_error("glGetUniformLocation [trans_x]");
    check_error(this->trans_x_loc >= 0, "Failed to obtain uniform location for trans_x.");

    // Start pos Y:
    this->start_pos_y_loc = glGetUniformLocation(this->shader_id, "start_pos_y");
    gl_check_error("glGetUniformLocation [start_pos_y]");
    check_error(this->start_pos_y_loc >= 0, "Failed to obtain uniform location for start_pos_y.");

    // Start pos X:
    this->start_pos_x_loc = glGetUniformLocation(this->shader_id, "start_pos_x");
    gl_check_error("glGetUniformLocation [start_pos_x]");
    check_error(this->start_pos_x_loc >= 0, "Failed to obtain uniform location for start_pos_x.");
}

void PlayerModel::init_values() {
    this->time = glfwGetTime();
    this->trans_x = 0;
    this->trans_y = 0;

    this->speed_x = 0.1;
    this->speed_y = 0;    

    glUniform1f(this->trans_y_loc, this->trans_y);
    gl_check_error("glUniform1f [trans_y]");

    glUniform1f(this->trans_x_loc, this->trans_x);
    gl_check_error("glUniform1f [trans_x]");

    glUniform1f(this->start_pos_x_loc, this->start_pos_x);
    gl_check_error("glUniform1f [start_pos_x]");

    glUniform1f(this->start_pos_y_loc, this->start_pos_y);
    gl_check_error("glUniform1f [start_pos_y]");
}

void PlayerModel::set_keys(Control control) {
    this->control = control;
}

void PlayerModel::add_line_model(std::shared_ptr<LineModel> line_model) {
    this->line_model = line_model;
}

Point PlayerModel::get_position() {
    return this->points.back();
}

std::vector<Point> PlayerModel::get_line_points() {
    return this->points;
}
