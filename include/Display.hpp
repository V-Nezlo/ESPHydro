/*!
@file
@brief Класс - имплементация для драйвера дисплея и тач контроллера
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#ifndef INCLUDE_DISPLAY_HPP_
#define INCLUDE_DISPLAY_HPP_

#include "EventBus.hpp"
#include "HardwareConfig.hpp"
#include <LovyanGFX.hpp>
#include <lvgl.h>

class LGFX : public lgfx::LGFX_Device {
	lgfx::Panel_ILI9488 _panel_instance;
	lgfx::Bus_SPI _bus_instance;
	lgfx::Light_PWM _light_instance;
	lgfx::Touch_GT911 _touch_instance;
public:
	LGFX(void)
	{
		{
		auto cfg = _bus_instance.config();    // バス設定用の構造体を取得します。
		cfg.spi_host = VSPI_HOST;     // 使用するSPIを選択  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
		// ※ ESP-IDFバージョンアップに伴い、VSPI_HOST , HSPI_HOSTの記述は非推奨になるため、エラーが出る場合は代わりにSPI2_HOST , SPI3_HOSTを使用してください。
		cfg.spi_mode = 0;             // SPI通信モードを設定 (0 ~ 3)
		cfg.freq_write = 40000000;    // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
		cfg.freq_read  = 16000000;    // 受信時のSPIクロック
		cfg.spi_3wire  = true;        // 受信をMOSIピンで行う場合はtrueを設定
		cfg.use_lock   = true;        // トランザクションロックを使用する場合はtrueを設定
		cfg.dma_channel = SPI_DMA_CH_AUTO; // 使用するDMAチャンネルを設定 (0=DMA不使用 / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=自動設定)
		// ※ ESP-IDFバージョンアップに伴い、DMAチャンネルはSPI_DMA_CH_AUTO(自動設定)が推奨になりました。1ch,2chの指定は非推奨になります。
		cfg.pin_sclk = Hardware::Display::kClkPin;            // SPIのSCLK
		cfg.pin_mosi = Hardware::Display::kMosiPin;            // SPIのMOSI
		cfg.pin_miso = Hardware::Display::kMisoPin;            // SPIのMISO (-1 = disable)
		cfg.pin_dc   = Hardware::Display::kDcPin;            // SPIのD/C  (-1 = disable)

		_bus_instance.config(cfg);    // 設定値をバスに反映します。
		_panel_instance.setBus(&_bus_instance);      // バスをパネルにセットします。
		}

		{ 
		auto cfg = _panel_instance.config();    // 表示パネル設定用の構造体を取得します。

		cfg.pin_cs           =    Hardware::Display::kCsPin;  // CSが接続されているピン番号   (-1 = disable)
		cfg.pin_rst          =    Hardware::Display::kRstPin;  // RSTが接続されているピン番号  (-1 = disable)
		cfg.pin_busy         =    Hardware::Display::kBusyPin;  // BUSYが接続されているピン番号 (-1 = disable)

		cfg.panel_width      =   320;  // 実際に表示可能な幅
		cfg.panel_height     =   480;  // 実際に表示可能な高さ
		cfg.offset_x         =     0;  // パネルのX方向オフセット量
		cfg.offset_y         =     0;  // パネルのY方向オフセット量
		cfg.offset_rotation  =     0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
		cfg.dummy_read_pixel =     8;  // ピクセル読出し前のダミーリードのビット数
		cfg.dummy_read_bits  =     1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
		cfg.readable         =  true;  // データ読出しが可能な場合 trueに設定
		cfg.invert           = false;  // パネルの明暗が反転してしまう場合 trueに設定
		cfg.rgb_order        = false;  // パネルの赤と青が入れ替わってしまう場合 trueに設定
		cfg.dlen_16bit       = false;  // 16bitパラレルやSPIでデータ長を16bit単位で送信するパネルの場合 trueに設定
		cfg.bus_shared       =  true;  // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

		_panel_instance.config(cfg);
		}

		{ 
		auto cfg = _light_instance.config();    // バックライト設定用の構造体を取得します。

		cfg.pin_bl = Hardware::Display::kBacklightPin;              // バックライトが接続されているピン番号
		cfg.invert = false;           // バックライトの輝度を反転させる場合 true
		cfg.freq   = 44100;           // バックライトのPWM周波数
		cfg.pwm_channel = Hardware::Display::kBacklightPwmChannel;          // 使用するPWMのチャンネル番号

		_light_instance.config(cfg);
		_panel_instance.setLight(&_light_instance);  // バックライトをパネルにセットします。
		}

		{ // タッチスクリーン制御の設定を行います。（必要なければ削除）
		auto cfg = _touch_instance.config();

		touchInit(Hardware::Touch::kRstPin, Hardware::Touch::kIntPin);

		cfg.x_min      = 0;    // タッチスクリーンから得られる最小のX値(生の値)
		cfg.x_max      = 319;  // タッチスクリーンから得られる最大のX値(生の値)
		cfg.y_min      = 0;    // タッチスクリーンから得られる最小のY値(生の値)
		cfg.y_max      = 479;  // タッチスクリーンから得られる最大のY値(生の値)

		cfg.pin_sclk = -1;     // SCLKが接続されているピン番号
		cfg.pin_mosi = -1;     // MOSIが接続されているピン番号
		cfg.pin_miso = -1;     // MISOが接続されているピン番号
		cfg.pin_cs   = -1;     //   CSが接続されているピン番号

		cfg.i2c_port = Hardware::Touch::kI2CPort;      // 使用するI2Cを選択 (0 or 1)
		cfg.i2c_addr = Hardware::Touch::kI2CAddr;   // I2Cデバイスアドレス番号
		cfg.pin_sda  = Hardware::Touch::kSdaPin;     // SDAが接続されているピン番号
		cfg.pin_scl  = Hardware::Touch::kSclPin;     // SCLが接続されているピン番号
		cfg.freq = 400000;     // I2Cクロックを設定

		_touch_instance.config(cfg);
		_panel_instance.setTouch(&_touch_instance);  // タッチスクリーンをパネルにセットします。
		}

		setPanel(&_panel_instance); // 使用するパネルをセットします。
	}

	void setBrightness(uint8_t aDuty)
	{
		return _light_instance.setBrightness(aDuty);
	}

private:
	void touchInit(int8_t aRstPin, int8_t aIntPin)
	{
        lgfx::pinMode(aRstPin, lgfx::pin_mode_t::output);
        lgfx::pinMode(aIntPin, lgfx::pin_mode_t::output);
        lgfx::gpio_lo(aRstPin);
        lgfx::gpio_lo(aIntPin);
        lgfx::delay(10);

        lgfx::gpio_hi(aIntPin);
        lgfx::delayMicroseconds(100);

        lgfx::gpio_hi(aRstPin);
        lgfx::delay(5);

        lgfx::gpio_lo(aIntPin);
        lgfx::delay(50);
	}
};

class DisplayDriver : public AbstractEventObserver {
	static LGFX display;

	static const uint32_t screenWidth  = 480;
	static const uint32_t screenHeight = 320;
	static lv_disp_draw_buf_t draw_buf;
	static lv_color_t buf[ screenWidth * 10 ];

public:
	virtual EventResult handleEvent(Event *e)
	{
		switch(e->type) {
			case EventType::NewBrightness:
				display.setBrightness(e->data.brightness);
				return EventResult::HANDLED;
				break;
			default:
				return EventResult::IGNORED;
		}
	}

	static void setupDisplay(void)
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

private:
/* Display flushing */
	static void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
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
	static void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
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

public:
	static void setupLvgl()
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
	}

	LGFX& lgfxGetDriver()
	{
		return display;
	}
};









#endif