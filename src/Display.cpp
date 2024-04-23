#include <LovyanGFX.hpp>
#include <Display.hpp>
#include <lvgl.h>

LGFX DisplayDriver::display;
lv_disp_draw_buf_t DisplayDriver::draw_buf;
lv_color_t DisplayDriver::buf[4800];

