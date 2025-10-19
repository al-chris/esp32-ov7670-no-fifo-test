//parts of his code are taken from
//https://github.com/igrr/esp32-cam-demo
//by Ivan Grokhotkov
//released under Apache License 2.0

#include "XClk.h"
#include "driver/ledc.h"
#include "driver/periph_ctrl.h"

bool ClockEnable(int pin, int Hz)
{
    periph_module_enable(PERIPH_LEDC_MODULE);

    ledc_timer_config_t timer_conf;
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_conf.timer_num = LEDC_TIMER_0;

    // Try to choose a duty resolution that allows the requested frequency.
    // APB clock is typically 80 MHz on ESP32.
    const uint32_t APB_CLK = 80000000UL;
    bool configured = false;
    esp_err_t err = ESP_OK;
    for (int bits = 1; bits <= 15; ++bits) {
        // maximum achievable frequency with this resolution is APB_CLK / (1 << bits)
        uint32_t maxFreq = (APB_CLK >> bits);
        if ((uint32_t)Hz <= maxFreq) {
            timer_conf.duty_resolution = (ledc_timer_bit_t)bits;
            timer_conf.freq_hz = Hz;
            esp_err_t err = ledc_timer_config(&timer_conf);
            if (err == ESP_OK) {
                configured = true;
                break;
            }
        }
    }

    // If not configured, try the lowest resolution with the highest possible freq
    if (!configured) {
        timer_conf.duty_resolution = LEDC_TIMER_1_BIT;
        timer_conf.freq_hz = APB_CLK >> 1; // best-effort fallback
        if (ledc_timer_config(&timer_conf) == ESP_OK) {
            configured = true;
        }
    }

    if (!configured) {
        return false;
    }

    // Debug info: report selected timer settings (helpful on serial monitor)
    // Serial may not be initialized yet in all contexts; guard with an existence
    // check for the symbol to avoid compile-time dependency in non-Arduino builds.
#ifdef Serial
    Serial.printf("XCLK: LEDC configured freq=%u duty_bits=%u\n", (unsigned)timer_conf.freq_hz, (unsigned)timer_conf.duty_resolution);
#endif

    ledc_channel_config_t ch_conf;
    ch_conf.channel = LEDC_CHANNEL_0;
    ch_conf.timer_sel = LEDC_TIMER_0;
    ch_conf.intr_type = LEDC_INTR_DISABLE;
    ch_conf.duty = 1;
    ch_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    ch_conf.gpio_num = pin;
    
    ch_conf.hpoint = 0;//added by me
    
    err = ledc_channel_config(&ch_conf);
    if (err != ESP_OK) {
        return false;
    }
    return true;
}

void ClockDisable()
{
    periph_module_disable(PERIPH_LEDC_MODULE);
}

