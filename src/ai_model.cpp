#include "ai_model.hpp"

#include <iostream>

#include "gl_calls.hpp"
#include "player_mesh.hpp"
#include "shader.hpp"

void AIModel::update(GLFWwindow* window) {
    this->arrow->update(window);
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
    glm::vec2 last_direction {this->speed_x, this->speed_y};

    int threshold = 5;

    // Check Borders
    if ((this->last_point.x + threshold) >= 13.5) {
        return direction = 1;
    } 
    if ((this->last_point.x - threshold) <= -18.75) {
        return direction = -1;
    }
    if ((this->last_point.y + threshold) >= 18.75) {
        return direction = -1;
    }
    if ((this->last_point.y - threshold) <= -18.75) {
        return direction = 1;
    }

    double smallest_distance = this->get_smallest_distance((GLfloat) 0);

    std::cout << "Smallest Distance: " << smallest_distance << std::endl;
    std::cout << "----" << std::endl;   

    if (smallest_distance < threshold && smallest_distance != -1) {
        double max_distance_left = this->max_look_ahead(-1);
        double max_distance_right = this->max_look_ahead(1);

        if (max_distance_left > max_distance_right) {
            direction = -1;
        } else  {
            direction = 1;
        }
    }

    return direction;
}

double AIModel::get_smallest_distance(GLfloat delta_angle) {

    glm::vec2 last_direction {this->speed_x, this->speed_y};
    
    if (delta_angle != 0) {
        GLfloat temp_speed_x = last_direction.x;
        GLfloat temp_speed_y = last_direction.y;

        last_direction.x = temp_speed_x * cos(delta_angle) - temp_speed_y * sin(delta_angle);
        last_direction.y = temp_speed_x * sin(delta_angle) + temp_speed_y * cos(delta_angle);
    }

    std::vector<glm::vec2> filtered_points;
    double smallest_distance = -1;
    glm::vec2 nearest_point;
    bool is_first = true;

    for (auto point : this->all_points) {
        auto centered_point = point - this->last_point;

        glm::vec2 vec1 {0, 1};
        glm::vec2 vec2 = last_direction;
        auto angle = get_angle(vec1, vec2);
        auto rotation = cross_product(vec1, vec2);
        
        if (rotation > 0) {
            angle = -1 * angle;
        } else if (rotation == 0 ) {
            angle = 0;
        }

        auto new_x = centered_point.x * cos(angle) - centered_point.y * sin(angle);
        auto new_y = centered_point.x * sin(angle) + centered_point.y * cos(angle);

        if (new_x < 1 && new_x > -1 && new_y > 0) {
            glm::vec2 new_point{new_x, new_y};
            auto distance = glm::length(this->last_point - point);
            if (is_first == true) {
                smallest_distance = distance;
                nearest_point = point;
                is_first = false;
            } else {
                if (distance < smallest_distance) {
                    smallest_distance = distance;
                    nearest_point = point;
                }
            }
        }
    }

    return smallest_distance;
}

GLfloat AIModel::get_delta_angle() {
    double new_time = glfwGetTime();
    double time_delta = new_time - this->time;
    double speed = 3;
    auto angle = static_cast<GLfloat>((speed * time_delta));
    return angle;
}

double AIModel::max_look_ahead(int direction) {
    auto delta_angle = this->get_delta_angle();
    double smallest_distance = -1;
    for (int i = 0; i<50; i++) {
        auto current_smallest_distance = this->get_smallest_distance(delta_angle);
        if (current_smallest_distance > smallest_distance) {
            smallest_distance = current_smallest_distance;
        }
        delta_angle += delta_angle*direction;        
    }
    
    if (direction == 1) {
        std::cout << "max_distance_right " << smallest_distance << std::endl;
    } else {
        std::cout << "max_distance_left " << smallest_distance << std::endl;
    }

    return smallest_distance;
}
