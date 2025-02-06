#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_err.h"
#include "blink.h"


// Define LED strip configuration
#define LED_GPIO 8           // WS2812B connected to GPIO 8
#define LED_STRIP_LEN 1      // Only 1 LED in the strip

#define DISCONNECTED_DELAY 100
#define CONNECTED_DELAY 600
#define CONNECTING_DELAY 300

connection_status_t connection_status = DISCONNECTED;

static led_strip_handle_t led_strip;  // LED strip handle

// Function to initialize the LED strip
void setup_rgb_led(void) {
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = LED_STRIP_LEN,  // Number of LEDs
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,  // Use the default RMT clock
        .resolution_hz = 10 * 1000 * 1000,  // 10 MHz resolution
        .flags.with_dma = false
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_ERROR_CHECK(led_strip_clear(led_strip));  // Turn off the LED initially
}

void disconnected_blink_pattern(void) {
    // Blink pattern when disconnected
    for (int i = 0; i < 5; i++) {
        led_strip_set_pixel(led_strip, 0, 30, 0, 0);  // Red
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        vTaskDelay(pdMS_TO_TICKS(DISCONNECTED_DELAY));

        led_strip_clear(led_strip);
        vTaskDelay(pdMS_TO_TICKS(DISCONNECTED_DELAY));
    }
}

void connected_blink_pattern(void) {
    // Blink pattern when connected
    for (int i = 0; i < 5; i++) {
        led_strip_set_pixel(led_strip, 0, 0, 30, 0);  // Green
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        vTaskDelay(pdMS_TO_TICKS(CONNECTED_DELAY));

        led_strip_clear(led_strip);
        vTaskDelay(pdMS_TO_TICKS(CONNECTED_DELAY));
    }
}

void connecting_blink_pattern(void) {
    // Blink pattern when connecting
    for (int i = 0; i < 5; i++) {
        led_strip_set_pixel(led_strip, 0, 30, 30, 0);  // Yellow
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        vTaskDelay(pdMS_TO_TICKS(CONNECTING_DELAY));

        led_strip_clear(led_strip);
        vTaskDelay(pdMS_TO_TICKS(CONNECTING_DELAY));
    }
}

void rgb_led_task(void *arg) {
    setup_rgb_led();

    while (1) {
        switch (connection_status) {
            case DISCONNECTED:
                disconnected_blink_pattern();
                break;
            case CONNECTED:
                connected_blink_pattern();
                break;
            case CONNECTING:
                connecting_blink_pattern();
                break;
            default:
                led_strip_clear(led_strip);
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
        }
    }
}