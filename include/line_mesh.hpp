#ifndef LINE_MESH_HPP
#define LINE_MESH_HPP

#include "mesh.hpp"
#include "user.hpp"
#include "point.hpp"

#include <vector>
#include <array>

class LineMesh: public Mesh {
   protected:
    std::vector<glm::vec2> points;
    std::vector<glm::vec2> line_points;
    glm::vec2 actual_point;
    glm::vec2 last_point;
    glm::vec2 second_last_point;
    glm::vec3 color;    
   public:
    LineMesh(glm::vec2 first_point, glm::vec3 color);
    ~LineMesh();
    virtual void draw() override;
    void set_points(std::vector<glm::vec2> points);
    void add_point(glm::vec2 point);
    void update();
    void clear();
};

#endif