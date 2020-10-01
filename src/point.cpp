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

Vector::Vector(GLfloat x, GLfloat y) {
    this->x = x;
    this->y = y;
}

double Vector::get_length() {
    double length = sqrt(pow(this->x, 2) + pow(this->y, 2));
    return length;
}

double Vector::dot_product(Vector v1, Vector v2) {
    double product = v1.x * v2.x + v1.y * v2.y;
    return product;
}

double Vector::cross_product(Vector v1, Vector v2) {
    double product = (double)(1000*v1.x * 1000*v2.y) - (double)(1000*v1.y * 1000*v2.x);
    return product;
}

double Vector::angle(Vector v1, Vector v2) {
    double dist_v1, dist_v2;
    dist_v1 = v1.get_length();
    dist_v2 = v2.get_length();
    double angle = 0;
    if ((dist_v1 != 0) && (dist_v2 != 0)) {
        angle = acos(
            Vector::dot_product(v1, v2) / (v1.get_length() * v2.get_length())
        );
    }
    return angle;
}
