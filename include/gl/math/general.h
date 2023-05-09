#pragma once

#include <type_traits>

namespace math {

    // every math type that has a state representing
    // zero needs to specialize this type with method:

    // static type zero() { ... } // which returns it
    template <typename type> struct zeroable {};

}

namespace math {

    struct zero_type_base {
        template <typename type>
        constexpr operator type() const { return zeroable<type>::zero(); }
    };

}

namespace math {

    inline constexpr zero_type_base zero = {};

}
