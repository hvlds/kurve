#include "ai_model.hpp"

#include <iostream>

#include "gl_calls.hpp"
#include "player_mesh.hpp"
#include "shader.hpp"

void AIModel::update(GLFWwindow* window) {
    // Update the direction of the directional arrow
    this->arrow->update(window);

    // Check if the goal was reached and assign a new one if true
    this->check_goal();

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

            // Check that the player is inside of the playground,
            // no collision with the border
            if (this->trans_x + this->start_pos_x >= 13.5 
                || this->trans_x + this->start_pos_x <= -18.75) {
                this->is_alive = false;
                return;
            }

            if (this->trans_y + this->start_pos_y >= 18.75 
                || this->trans_y + this->start_pos_y <= -18.75) {
                this->is_alive = false;
                return;
            }

            this->time = new_time;
            GLfloat angle_diff = 0;

            // Update the angle diferential
            int direction = this->plan();
            if (direction == 1) {
                angle_diff = static_cast<GLfloat>((-speed * time_delta));
            } else if (direction == -1){
                angle_diff = static_cast<GLfloat>((speed * time_delta));
            } else {
                angle_diff = 0;
            }

            glm::vec2 speed_vec{this->speed_x, this->speed_y};

            if (angle_diff != 0) {
                // Rotate the speed vector
                GLfloat temp_speed_x = this->speed_x;
                GLfloat temp_speed_y = this->speed_y;

                this->speed_x = temp_speed_x * cos(angle_diff) - temp_speed_y * sin(angle_diff);
                this->speed_y = temp_speed_x * sin(angle_diff) + temp_speed_y * cos(angle_diff);
            }

            this->trans_x += this->speed_x;
            this->trans_y += this->speed_y;    

            glm::vec2 point{
                this->trans_x + this->start_pos_x,
                this->trans_y + this->start_pos_y
            };

            if (this->lines.back()->get_points().size() 
                < static_cast<std::size_t>(this->random_length)) {
                // Draw the line
                this->points.push_back(point);
                this->lines.back()->add_point(point);
                this->lines.back()->update(window);
            } else {
                if (this->blank_count <= 15) {
                    // Still moving, but without drawing a line
                    this->blank_count++;
                } else {
                    // Initialize a new line and add the first point
                    auto line = std::make_shared<LineModel>(point, this->color);
                    this->lines.push_back(line);

                    this->lines.back()->add_point(point);
                    this->lines.back()->update(window);

                    // Set the blank count to null and add a new random length
                    this->blank_count = 0;
                    this->random_length = 70 + (rand() % 120);
                }
            }
            
            // Set the current position of the player
            this->last_point = point;

            // Update the uniform:
            glUseProgram(this->shader_id);

            glUniform1f(this->trans_y_loc, this->trans_y);
            gl_check_error("glUniform1f [trans_y]");

            glUniform1f(this->trans_x_loc, this->trans_x);
            gl_check_error("glUniform1f [trans_x]");
        }
    }
}

void AIModel::set_all_points(std::vector<glm::vec2> all_points) {
    this->all_points = all_points;
}

int AIModel::plan() {
    int direction = 0;

    // this->grid->populate(this->all_points);
    // this->grid->set_player(this->last_point, glm::vec2(this->speed_x, this->speed_y));
    
    // auto start_vec = this->grid->get_coordinates(last_point.x, last_point.y); 
    // auto goal_vec = this->grid->get_coordinates(goal.x, goal.y);

    // if (this->counter_plan > 10) {
    //     std::cout << "Start planning" << std::endl;
    //     auto next_cell = this->grid->get_next_cell(start_vec, goal_vec);
    //     std::cout << "Next cell: " << next_cell.x << "," << next_cell.y << std::endl;
    //     this->counter_plan = 0;
    // } else {
    //     this->counter_plan++;
    // }

    return direction;
}

void AIModel::set_new_goal() {
    int random_x= -100 + (rand() % 200);
    int random_y= -100 + (rand() % 200);
    GLfloat x = (GLfloat) random_x / 10;
    GLfloat y = (GLfloat) random_y / 10;
    
    glm::vec2 new_goal {x, y};
#ifdef DEBUG
    std::cout << "New Goal for AI-" << this->id << ": " << x << " , " << y << std::endl;
#endif

    this->goal = new_goal;
}

void AIModel::check_goal() {
    double distance = glm::length(this->last_point - this->goal);
    if (distance < 0.2) {
#ifdef DEBUG
        std::cout << "Goal reached!" << std::endl;
#endif
        this->set_new_goal();
    }
}

void AIModel::clear() {
    // Delete the trace of positions (points)
    this->points.clear();

    // Clear the lines of the player
    for (auto line : this->lines) {
        line->clear();
    }

    // Clear the vector with the lines
    this->lines.clear();

    // Clear the grid
    // this->grid->clear();
}
