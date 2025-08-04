#include <driver/i2s.h>
#include <math.h> // for sin()

// ✅ SAFE GPIOs for ESP32-CAM (avoid 14, 12, 13, 0)
#define I2S_BCLK  16  // Bit Clock
#define I2S_LRCK  2   // Word Select (LRC)
#define I2S_DOUT  15  // Data Output

void setup() {
  Serial.begin(115200);
  Serial.println("Booting I2S test...");

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRCK,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

void loop() {
  static uint32_t phase = 0;
  const int sample_rate = 44100;
  const int frequency = 440; // A4
  int16_t sample;

  for (int i = 0; i < 64; i++) {
    float t = (float)phase / sample_rate;
    sample = (int16_t)(10000 * sinf(2 * PI * frequency * t));

    int32_t stereo_sample = (sample << 16) | (sample & 0xFFFF); // Left + Right
    size_t bytes_written;
    i2s_write(I2S_NUM_0, &stereo_sample, sizeof(stereo_sample), &bytes_written, portMAX_DELAY);
    phase++;
  }

  delay(1); // Prevent watchdog timeout
}
