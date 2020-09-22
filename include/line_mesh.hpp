#ifndef LINE_MESH_HPP
#define LINE_MESH_HPP

#include "mesh.hpp"
#include "user.hpp"
#include "point.hpp"

#include <vector>
#include <array>

class LineMesh: public Mesh {
   protected:
    std::vector<Point> points;
    std::vector<Point> line_points;
    Point actual_point;
    Point last_point;
    Point second_last_point;
    std::array<GLubyte, 3> color;    
   public:
    LineMesh(Point first_point, std::array<GLubyte, 3> color);
    virtual void draw() override;
    void set_points(std::vector<Point> points);
    void add_point(Point point);
    void update();
};

#endif