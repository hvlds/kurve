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
    this->is_alive = true;
    
    // Set an initial random length for the first line
    this->random_length = 70 + (rand() % 120);
    
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
    this->lines.push_back(line);
}

void PlayerModel::draw() {
    glUseProgram(this->shader_id);
    this->mesh->draw();

    for (auto line : this->lines) {
        line->draw();
    }
}    

void PlayerModel::update(GLFWwindow* window) {
    // Update the time and calculate the delta:
    auto user_data = (user_data_t*) glfwGetWindowUserPointer(window);
    GameState game_state = user_data->game_state;
    if (game_state == GAME_PAUSE) {
        double new_time = glfwGetTime();
        this->time = new_time;
    } else if (game_state == GAME_ACTIVE) {
        if (this->is_alive == true) {
            glUseProgram(this->shader_id);
            double new_time = glfwGetTime();
            double time_delta = new_time - this->time;
            double speed = 3.0; 

            // Check that the player is inside of the yellow limit
            if (this->trans_x + this->start_pos_x >= 13.5 
                || this->trans_x + this->start_pos_x <= -18.5) {
                this->is_alive = false;
                return;
            }

            if (this->trans_y + this->start_pos_y >= 18.5 
                || this->trans_y + this->start_pos_y <= -18.5) {
                this->is_alive = false;
                return;
            }

            this->time = new_time;
            GLfloat angle_diff = 0;

            // Update the angle diferential
            int right_state = glfwGetKey(window, this->control.right_key);
            if (right_state == GLFW_PRESS) {
                angle_diff = static_cast<GLfloat>((-speed * time_delta));        
            }

            int left_state = glfwGetKey(window, this->control.left_key);
            if (left_state == GLFW_PRESS) {
                angle_diff = static_cast<GLfloat>((speed * time_delta));
            }

            Vector speed_vec{this->speed_x, this->speed_y};

            if (angle_diff != 0) {
                // Rotate the speed vector
                GLfloat temp_speed_x = this->speed_x;
                GLfloat temp_speed_y = this->speed_y;

                this->speed_x = temp_speed_x * cos(angle_diff) - temp_speed_y * sin(angle_diff);
                this->speed_y = temp_speed_x * sin(angle_diff) + temp_speed_y * cos(angle_diff);
            }

            this->trans_x += this->speed_x;
            this->trans_y += this->speed_y;    

            Point point{
                this->trans_x + this->start_pos_x,
                this->trans_y + this->start_pos_y
            };

            if (this->lines.back()->get_points().size() < this->random_length) {
                // Draw the line
                Point last_point = this->points.back();
                this->points.push_back(point);
                this->lines.back()->add_point(point);
                this->lines.back()->update(window);
            } else {
                if (this->blank_count <= 15) {
                    // Still moving, but without drawing a line
                    this->blank_count++;
                } else {
                    // Initialize a new line and add the first point
                    Point last_point = this->points.back();
                    auto line = std::make_shared<LineModel>(point, this->color);
                    this->lines.push_back(line);

                    this->lines.back()->add_point(point);
                    this->lines.back()->update(window);

                    // Set the blank count to null and add a new random length
                    this->blank_count = 0;
                    this->random_length = 70 + (rand() % 120);
                }
            }

            // Update the uniform:
            glUseProgram(this->shader_id);

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

    this->speed_x = 0.09;
    this->speed_y = 0;

    // Generate a random initial speed vector
    GLfloat temp_speed_x = this->speed_x;
    GLfloat temp_speed_y = this->speed_y;

    int random_value= -300 + (rand() % 500);
    double angle = random_value / 10;

    this->speed_x = temp_speed_x * cos(angle) - temp_speed_y * sin(angle);
    this->speed_y = temp_speed_x * sin(angle) + temp_speed_y * cos(angle);    

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


Point PlayerModel::get_position() {
    return this->points.back();
}

std::vector<Point> PlayerModel::get_line_points() {
    return this->points;
}

int PlayerModel::get_id() {
    return this->id;
}

void PlayerModel::clear() {
    // Delete the trace of positions (points)
    this->points.clear();

    // Clear the lines of the player
    for (auto line : this->lines) {
        line->clear();
    }

    // Clear the vector with the lines
    this->lines.clear();
}

void PlayerModel::set_position(Point point) {
    glUseProgram(this->shader_id);
    this->points.push_back(point);

    this->blank_count = 0;
    this->last_point = point;
    this->start_pos_x = point.x;
    this->start_pos_y = point.y;
    
    this->init_values();

    auto line = std::make_shared<LineModel>(point, this->color);
    this->lines.push_back(line);
}