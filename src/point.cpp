#include "point.hpp"

double get_distance(Point p1, Point p2) {
    double distance;
    distance = sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
    return distance;
}