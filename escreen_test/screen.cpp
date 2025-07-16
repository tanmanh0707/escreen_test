#include "common.h"

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7789    _panel_instance;  // Change this to your LCD driver
  lgfx::Bus_SPI         _bus_instance;   // Parallel 8-bit bus
  lgfx::Light_PWM       _light_instance;
  lgfx::Touch_CST816S   _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI3_HOST;
      cfg.spi_mode = 0;             // SPI通信モードを設定 (0 ~ 3)
      cfg.freq_write = 80000000;    // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
      cfg.spi_3wire  = true;        // 受信をMOSIピンで行う場合はtrueを設定
      cfg.use_lock   = true;        // トランザクションロックを使用する場合はtrueを設定
      cfg.dma_channel = SPI_DMA_CH_AUTO; // 使用するDMAチャンネルを設定 (0=DMA不使用 / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=自動設定)
      // ※ ESP-IDFバージョンアップに伴い、DMAチャンネルはSPI_DMA_CH_AUTO(自動設定)が推奨になりました。1ch,2chの指定は非推奨になります。
      cfg.pin_sclk = TFT_SCK;            // SPIのSCLKピン番号を設定
      cfg.pin_mosi = TFT_MOSI;            // SPIのMOSIピン番号を設定
      cfg.pin_miso = -1;            // SPIのMISOピン番号を設定 (-1 = disable)
      cfg.pin_dc   = TFT_DC;            // SPIのD/Cピン番号を設定  (-1 = disable)

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    { // Configure display panel
      auto cfg = _panel_instance.config();
      cfg.pin_cs = TFT_CS;
      cfg.pin_rst = TFT_RST;
      // cfg.pin_bl = TFT_BLK;
      cfg.pin_busy = -1;
      // cfg.backlight_level = true; // Set to `false` if backlight is inverted

      cfg.panel_width      =   SCREEN_WIDTH;  // 実際に表示可能な幅
      cfg.panel_height     =   SCREEN_HEIGHT;  // 実際に表示可能な高さ
      cfg.offset_x         =     0;  // パネルのX方向オフセット量
      cfg.offset_y         =     0;  // パネルのY方向オフセット量
      cfg.offset_rotation  =     0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.dummy_read_pixel =     8;  // ピクセル読出し前のダミーリードのビット数
      cfg.dummy_read_bits  =     1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
      cfg.readable         =  true;  // データ読出しが可能な場合 trueに設定
      cfg.invert           = true;  // パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order        = false;  // パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit       = false;  // 16bitパラレルやSPIでデータ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared       =  true;  // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

      _panel_instance.config(cfg);
    }

    { // バックライト制御の設定を行います。（必要なければ削除）
      auto cfg = _light_instance.config();    // バックライト設定用の構造体を取得します。

      cfg.pin_bl = TFT_BLK;              // バックライトが接続されているピン番号
      cfg.invert = false;           // バックライトの輝度を反転させる場合 true
      cfg.freq   = 44100;           // バックライトのPWM周波数
      cfg.pwm_channel = 7;          // 使用するPWMのチャンネル番号

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // バックライトをパネルにセットします。
    }

    { // タッチスクリーン制御の設定を行います。（必要なければ削除）
      auto cfg = _touch_instance.config();

      cfg.x_min      = 0;    // タッチスクリーンから得られる最小のX値(生の値)
      cfg.x_max      = SCREEN_WIDTH - 1;  // タッチスクリーンから得られる最大のX値(生の値)
      cfg.y_min      = 0;    // タッチスクリーンから得られる最小のY値(生の値)
      cfg.y_max      = SCREEN_HEIGHT - 1;  // タッチスクリーンから得られる最大のY値(生の値)
      cfg.pin_int    = TOUCH_INT;   // INTが接続されているピン番号
      cfg.bus_shared = true; // 画面と共通のバスを使用している場合 trueを設定
      cfg.offset_rotation = 0;// 表示とタッチの向きのが一致しない場合の調整 0~7の値で設定

// I2C接続の場合
      cfg.i2c_port = 1;      // 使用するI2Cを選択 (0 or 1)
      cfg.i2c_addr = 0x15;   // I2Cデバイスアドレス番号
      cfg.pin_sda  = TOUCH_SDA;     // SDAが接続されているピン番号
      cfg.pin_scl  = TOUCH_SCL;     // SCLが接続されているピン番号
      cfg.pin_rst  = TOUCH_RST;
      cfg.freq = 400000;     // I2Cクロックを設定

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);  // タッチスクリーンをパネルにセットします。
    }

    setPanel(&_panel_instance);
  }
};

LGFX _tft;
static lv_display_t *lvDisplay;
const unsigned int lvBufferSize = SCREEN_WIDTH * 30;
uint8_t lvBuffer[2][lvBufferSize];

void LVGL_Init();

static uint32_t my_tick(void)
{
  return millis();
}

void LVGL_Flush(lv_display_t *display, const lv_area_t *area, unsigned char *data)
{
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  lv_draw_sw_rgb565_swap(data, w * h);

  if (_tft.getStartCount() == 0)
  {
    _tft.endWrite();
  }
  _tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t *)data);
  lv_display_flush_ready(display); /* tell lvgl that flushing is done */
}

void my_log_cb(lv_log_level_t level, const char * buf)
{
  log_i("%s", buf);
}

void SCREEN_Init(void)
{
  _tft.begin();
  _tft.initDMA();
  _tft.fillScreen(TFT_GREEN);

  LVGL_Init();
}

void SCREEN_SetAddrWindow(int32_t x, int32_t y, int32_t w, int32_t h)
{
  _tft.setAddrWindow(x, y, w, h);
}

void SCREEN_PushPixelsDMA(const uint16_t* pixels, uint32_t len, bool swap)
{
  _tft.pushPixelsDMA(pixels, len, swap);
}

void SCREEN_Update()
{
  lv_timer_handler();
}

static void set_x(void * var, int32_t v)
{
  lv_obj_set_x((lv_obj_t *)var, v);
}

void lv_example_gif(void)
{
  LV_IMAGE_DECLARE(img_bulb_gif);
  lv_obj_t * img;

  img = lv_gif_create(lv_screen_active());
  lv_gif_set_src(img, &img_bulb_gif);
  lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_obj_set_style_text_color(label, lv_color_hex(0x00ff00), LV_PART_MAIN);
  lv_obj_add_flag(label, LV_OBJ_FLAG_FLOATING);
  lv_label_set_text(label, "This is test text");
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_update_layout(lv_screen_active());

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, label);
  lv_anim_set_values(&a, lv_obj_get_width(lv_screen_active()), -lv_obj_get_width(label));
  lv_anim_set_time(&a, 10000);
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_exec_cb(&a, set_x);
  lv_anim_start(&a);
}


void LVGL_Init()
{
  lv_init();
  lv_tick_set_cb(my_tick);
  // lv_log_register_print_cb(my_log_cb);
  lvDisplay = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_color_format(lvDisplay, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(lvDisplay, LVGL_Flush);
  lv_display_set_buffers(lvDisplay, lvBuffer[0], lvBuffer[1], lvBufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

  lv_example_gif();
}
