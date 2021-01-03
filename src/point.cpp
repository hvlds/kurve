#include "point.hpp"

// Vector::Vector(glm::vec2 start, glm::vec2 end) {
//     this->x = end.x - start.x;
//     this->y = end.y - start.y;
// }

double get_length(glm::vec2 v) {
    double length = sqrt(pow(v.x, 2) + pow(v.y, 2));
    return length;
}

double dot_product(glm::vec2 v1, glm::vec2 v2) {
    double product = v1.x * v2.x + v1.y * v2.y;
    return product;
}

double cross_product(glm::vec2 v1, glm::vec2 v2) {
    double product = (double)(1000*v1.x * 1000*v2.y) - (double)(1000*v1.y * 1000*v2.x);
    return product;
}

double get_angle(glm::vec2 v1, glm::vec2 v2) {
    double dist_v1, dist_v2;
    dist_v1 = get_length(v1);
    dist_v2 = get_length(v2);
    double angle = 0;
    if ((dist_v1 != 0) && (dist_v2 != 0)) {
        angle = acos(
            dot_product(v1, v2) / (get_length(v1) * get_length(v2))
        );
    }
    return angle;
}
