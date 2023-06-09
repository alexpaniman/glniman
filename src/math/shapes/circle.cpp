#include "circle.h"

bool circle::intersect_circle(const circle &other, std::vector<math::vec2> &points) const noexcept {
    float dx = other.center.x() - center.x();
    float dy = other.center.y() - center.y();

    // distance between centers:
    float distance = sqrtf(pow(dx, 2) + pow(dy, 2));

    if (distance > (other.radius + radius))
        return false;

    float dl = (pow(radius, 2) - pow(other.radius, 2) + distance * distance)
        / (2 * distance);

    float height = sqrt(radius * radius - pow(dl, 2));
    math::vec middle = (center - other.center) * (dl / distance) - center;

    points.emplace_back(
        - middle.x() - height * dy / distance,
        - middle.y() + height * dx / distance
    );

    points.emplace_back(
        - middle.x() + height * dy / distance,
        - middle.y() - height * dx / distance
    );

    return true;
}

template <bool fill> 
static inline void draw_or_fill(const circle &circle, gl::drawing_manager mgr) {
    double segments = ceil(std::max(20. * log(circle.radius / 0.003 + 1), 8.));
    double step = 2 * M_PI / segments;

    // Rotate current and previous around circle's center
    math::vec2 current = circle.center + math::vec2 { 0.f, static_cast<float>(circle.radius) };
    math::vec2 previous = { 0.f, 0.f };

    double max_angle = ceil(ceil(2 * M_PI / step) * step);
    for (double angle = 0.0; angle <= max_angle; angle += step) {
        current.rotate(circle.center, step);

        if (angle != 0.0) {
            if constexpr (fill)
                mgr.draw_triangle(current, circle.center, previous);
            else {
                mgr.draw_line(current, previous);
            }
        }

        previous = current;
    }
}

math::vec2 circle::project(math::vec2 point) const noexcept {
    math::vec direction = (point - center).normalize();
    return center + radius * direction;
}

void circle::draw(gl::drawing_manager mgr) const { draw_or_fill</* fill = */false>(*this, mgr); }
void circle::fill(gl::drawing_manager mgr) const {
    draw_or_fill</* fill = */true>(*this, mgr);
}
