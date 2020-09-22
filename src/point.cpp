#include "point.hpp"

double Point::get_distance(Point p1, Point p2) {
    double distance;
    distance = sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
    return distance;
}

Vector::Vector(Point start, Point end) {
    this->x = end.x - start.x;
    this->y = end.y - start.y;
}

double Vector::get_lenght() {
    double length = sqrt(pow(this->x, 2) + pow(this->y, 2));
    return length;
}

GLfloat Vector::dot_product(Vector v1, Vector v2) {
    GLfloat product = v1.x * v2.x + v1.y * v2.y;
    return product;
}

GLfloat Vector::cross_product(Vector v1, Vector v2) {
    GLfloat product = (v1.x * v2.y) - (v1.y * v2.x);
    return product;
}
