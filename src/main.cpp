#include <LovyanGFX.hpp>
#include <lvgl.h>
#include "GpioWrapper.hpp"
#include "esp_log.h"
#include "Display.hpp"
#include "GT911Initializer.hpp"
#include "UI.hpp"
#include "HydroRS.hpp"

LGFX display;

void setupDisplay(void)
{
	display.init();
	display.setTextSize((std::max(display.width(), display.height()) + 255) >> 8);

	if (display.touch()) {
		if (display.width() < display.height()) display.setRotation(display.getRotation() ^ 1);

		std::uint16_t fg = TFT_WHITE;
		std::uint16_t bg = TFT_BLACK;
		if (display.isEPD()) std::swap(fg, bg);
	}
}

static const uint32_t screenWidth  = 480;
static const uint32_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
   uint32_t w = ( area->x2 - area->x1 + 1 );
   uint32_t h = ( area->y2 - area->y1 + 1 );

   display.startWrite();
   display.setAddrWindow( area->x1, area->y1, w, h );
   display.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
   display.endWrite();

   lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
	uint16_t touchX, touchY;
	bool touched = display.getTouch( &touchX, &touchY);
	if( !touched ) {
		data->state = LV_INDEV_STATE_REL;
	} else {
		data->state = LV_INDEV_STATE_PR;

		data->point.x = touchX;
		data->point.y = touchY;
	}
}

void lvlgSetup()
{
	lv_init();
	lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

	/*Initialize the display*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);

	/*Change the following line to your display resolution*/
	disp_drv.hor_res = screenWidth;
	disp_drv.ver_res = screenHeight;
	disp_drv.flush_cb = my_disp_flush;
	disp_drv.draw_buf = &draw_buf;
	lv_disp_drv_register(&disp_drv);

	/*Initialize the (dummy) input device driver*/
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_touchpad_read;
	lv_indev_drv_register(&indev_drv);

	uiInit();
}

void loop()
{
	lv_timer_handler(); /* let the GUI do its work */
	lgfx::delay(5);
	lv_tick_inc(5);
}

extern "C"
void app_main()
{
	GT911Initializer::init();
	setupDisplay();
	lvlgSetup();

	while(true) {
		loop();
	}

}
