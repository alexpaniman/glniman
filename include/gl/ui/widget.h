#pragma once

#include "drawing-manager.h"
#include "rect.h"
#include "vec.h"
#include <functional>
#include <limits>
#include <memory>

struct mouse_event {
    enum its_kind {
        press,
        click,
        hold
    };

    math::vec2 point;
    its_kind kind;
};

struct biased_size_range {
    math::svec2 min, preffered, max;

    biased_size_range intersect(biased_size_range other) {
        biased_size_range new_range = *this;

        // TODO: improve!

        new_range.min.x() = std::max<std::size_t>(new_range.min.x(), other.min.x());
        new_range.min.y() = std::max<std::size_t>(new_range.min.y(), other.min.y());

        // prefer larget sizes:
        new_range.preffered.x() =
            std::max<std::size_t>(new_range.preffered.x(), other.preffered.x());
        new_range.preffered.y() =
            std::max<std::size_t>(new_range.preffered.y(), other.preffered.y());

        new_range.max.x() = std::min<std::size_t>(new_range.max.x(), other.max.x());
        new_range.max.y() = std::min<std::size_t>(new_range.max.y(), other.max.y());

        return new_range; // TODO: improve this function
    }
};


struct space_requirements {
    double aspect_ratio;
    biased_size_range bounding_box;

    // TODO: possibly, allow for a more complex size
    //       distribution... maybe, like so:
    // int (*size_desirability)(math::svec2 size);
};

struct widget {
public:
    virtual void draw(gl::drawing_manager mgr) = 0;

    // we don't want to recalculate layout every time
    // as it can possibly be expensive, so let's introduce
    // a separate step for it:
    virtual bool resize(math::svec2 new_size);

    virtual space_requirements get_space_requirements() = 0;

    // TODO: 
    virtual bool on_mouse_event() = 0;
    virtual bool on_focus_acquired() = 0;

    virtual ~widget() = 0;
};


struct weighted_widget {
    widget* operator->() { return widget.get(); }

    std::unique_ptr<widget> widget;
    double weight = 0.0, stretch = 1.0;
};

struct positioned_widget {
    widget* operator->() { return widget.widget.get(); }

    math::rectangle bounding_box;
    weighted_widget widget;

    void draw(gl::drawing_manager mgr) {
        widget->draw(mgr.with_applied(bounding_box));
    }
};




class box: public widget {
public:
    box(std::same_as<weighted_widget> auto&&... widgets) {
        math::rectangle default_size = math::zero; // TODO: make constexpr

        ([&] {
            weighted_widget widget {
                default_size,
                std::move(widgets)
            };

            widgets_.emplace(widgets.coefficient, std::move(widget));
        }, ...);
    }

    void draw(gl::drawing_manager mgr) override {
        for (auto &&[coefficient, positioned_widgets]: widgets_)
            for (auto &&positioned_widget: positioned_widgets)
                positioned_widget.draw(mgr);
    }

    bool resize(math::svec2 new_size) override {
        // step 1: find appropriate height 
        biased_size_range range = // TODO: this is rediculous!
            widgets_.begin()->second.begin()->widget->get_space_requirements().bounding_box;

        // can get_space_requirements be slow? TODO: maybe don't recompute

        for (auto &&[coefficient, positioned_widgets]: widgets_)
            for (auto &&widget: positioned_widgets) {
                auto space = widget->get_space_requirements();
                range = range.intersect(space.bounding_box);
            }

        // TODO: check height sufficience

        int available_width = new_size.x(); // can possibly be < 0

        // step 2: propagate min size
        for (auto &&[coefficient, positioned_widgets]: widgets_) {
            for (auto &&positioned_widget: positioned_widgets) {
                auto space = positioned_widget->get_space_requirements();

                available_width -= space.bounding_box.min.x();

                // FIXME:
                positioned_widget.bounding_box.x1 = space.bounding_box.min;
            }
        }

        if (available_width < 0) // not enough space even for smallest
            return false; // TODO: renormalize sizes anyway

        // step 3: propagate desired space
        for (auto &&[coefficent, positioned_widgets]: widgets_) {

            int required_size = 0;
            for (auto &&positioned_widget: positioned_widgets) {
                auto space = positioned_widget->get_space_requirements();
                required_size += space.bounding_box.preffered.x();
            }

            // use whatever we have
            if (required_size > available_width)
                required_size = available_width;

            // TODO: assert sum of stretches is 1
            for (auto &&positioned_widget: positioned_widgets) {
                double coefficient = positioned_widget.widget.stretch;
                positioned_widget.bounding_box.x1 = available_width * coefficient;
            }

            available_width -= required_size;
        }
    }


    space_requirements get_space_requirements() override { return requirements_; }

private:
    // sort all widgets by priority (from high to low)
    std::map<double, std::vector<positioned_widget>, std::greater<double>> widgets_;
    space_requirements requirements_;
};
