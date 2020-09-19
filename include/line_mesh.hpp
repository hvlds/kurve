#ifndef LINE_MESH_HPP
#define LINE_MESH_HPP

#include "mesh.hpp"
#include "user.hpp"
#include "point.hpp"

#include <vector>

class LineMesh: public Mesh {
   protected:
    std::vector<Point> points;    
   public:
    LineMesh(Point first_point);
    virtual void draw() override;
    void set_points(std::vector<Point> points);
    void add_point(Point point);
    void update();
};

#endif