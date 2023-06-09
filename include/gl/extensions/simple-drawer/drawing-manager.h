#pragma once

#include "axes.h"
#include "colored-vertex.h"
#include "opengl-setup.h"
#include "rect.h"
#include "vertex-vector-array.h"
#include "vec.h"

namespace gl {

    math::vec3 hex(const char* color);

    class drawing_manager {
    public:
        // Uses black color by default
        drawing_manager(gl::vertex_vector_array<colored_vertex>& vertices):
            m_vertices(vertices),
            m_stroke_color(0.0f, 0.0f, 0.0f),
            m_fill_color  (0.0f, 0.0f, 0.0f) {}

        // ==> Get     current settings:

        math::vec3 stroke_color() { return m_stroke_color; }
        math::vec3   fill_color() { return m_fill_color;   }

        // ==> Control current settings:

        void set_outer_color(math::vec3 color);
        void set_inner_color(math::vec3 color);

        void set_width(float width);
        void set_axes(math::axes axes);
        void add_axes(math::axes axes);

        drawing_manager with_applied(math::axes axes);

        // ==> Draw shapes:

        void draw_interpolated_triangle(colored_vertex p0, colored_vertex p1, colored_vertex p2);

        void draw_triangle(math::vec2 p0, math::vec2 p1, math::vec2 p2);

        void draw_rectangle(math::vec2 x0, math::vec2 x1);
        void draw_rectangle(math::rectangle rect) { draw_rectangle(rect.x0, rect.x1); }

        void draw_line(math::vec2 from, math::vec2 to);
        void draw_antialiased_line(math::vec2 from, math::vec2 to,
                                   float antialiasing_level = 0.8f);

        void draw_vector(math::vec2 from, math::vec2 to);

    private:
        gl::vertex_vector_array<colored_vertex>& m_vertices;

    public:
        // ==> Current settings:
        math::axes m_axes;

        math::rectangle screen() {
            return math::rectangle {
                m_axes.get_world_coordinates({ -1.f, -1.f }),
                m_axes.get_world_coordinates({  1.f,  1.f }),
            };
        }

    private:
        math::vec3 m_stroke_color;
        math::vec3 m_fill_color;

        float m_width;
    };

}
