#ifndef GT911INITIALIZER_HPP_
#define GT911INITIALIZER_HPP_

#include <LovyanGFX.hpp>

class GT911Initializer {
public:
    static void init()
    {
        #define TOUCH_RST 21
        #define TOUCH_INT 23

        lgfx::pinMode(TOUCH_RST, lgfx::pin_mode_t::output);
        lgfx::pinMode(TOUCH_INT, lgfx::pin_mode_t::output);
        lgfx::gpio_lo(TOUCH_RST);
        lgfx::gpio_lo(TOUCH_INT);
        lgfx::delay(10);

        lgfx::gpio_hi(TOUCH_INT);
        lgfx::delayMicroseconds(100);

        lgfx::gpio_hi(TOUCH_RST);
        lgfx::delay(5);

        lgfx::gpio_lo(TOUCH_INT);
        lgfx::delay(50);
    }
};

#endif