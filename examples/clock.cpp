#include "gl.h"

class my_window: public gl::simple_drawing_window {
public:
    using gl::simple_drawing_window::simple_drawing_window;

    void loop_draw(gl::drawing_manager mgr) override {
        // ==> Background
        mgr.set_outer_color(gl::hex("#211c1f"));
        mgr.draw_rectangle(mgr.screen());

        constexpr float clock_radius = 0.8f;

        constexpr int num_major_ticks = 12;
        constexpr int num_minor_ticks = num_major_ticks * 5;

        // ==> Minor ticks
        mgr.set_outer_color(gl::hex("#a6d975"));
        mgr.set_width(0.005f); // Minor tick width
        math::vec2 minor_tick_end = { clock_radius, 0.0f };
        for (int i = 0; i < num_minor_ticks; ++ i) {
            mgr.draw_line(minor_tick_end, minor_tick_end * 0.95);
            minor_tick_end.rotate(2 * M_PI / num_minor_ticks);
        }

        // ==> Major ticks
        mgr.set_outer_color(gl::hex("#e5334c"));
        mgr.set_width(0.010f);
        math::vec2 major_tick_end = { clock_radius, 0.0f };
        for (int i = 0; i < 12; ++ i) {
            mgr.draw_line(major_tick_end, major_tick_end * 0.85);
            major_tick_end.rotate(2 * M_PI / num_major_ticks);
        }

        // ==> Clock face
        mgr.set_outer_color(gl::hex("#e5334c"));
        mgr.set_width(0.02f);
        circle({ 0.0f, 0.0f }, clock_radius).draw(mgr);


        // ==> Get current time
        time_t now = time(nullptr);
        tm *gmtm = localtime(&now);

        // 12 o'clock direction
        const math::vec line = { 0.0f, -0.7f };

        // Arrow and cap color:
        mgr.set_outer_color(gl::hex("#a6d975"));

        // ==> Arrows
        mgr.set_width(0.010f); // Hours   arrow width
        mgr.draw_line({ 0.0f, 0.0f }, line.rotated(gmtm->tm_hour * 2 * M_PI / 12.f));

        mgr.set_width(0.007f); // Minutes arrow width
        mgr.draw_line({ 0.0f, 0.0f }, line.rotated(gmtm->tm_min  * 2 * M_PI / 60.f));

        mgr.set_width(0.003f); // Seconds arrow width
        mgr.draw_line({ 0.0f, 0.0f }, line.rotated(gmtm->tm_sec  * 2 * M_PI / 60.f));
        
        // ==> Arrow cap
        circle({ 0.0f, 0.0f }, 0.02f).fill(mgr);
    }
};

int main() {
    my_window window(1080, 1080, "Clock | OpenGL");
    window.draw_loop();
}
