#include "connect.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "blink.h"
#include "monitor.h"


static const char *TAG = "main task";

void connection_manager_task(void *arg) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 sec
        connection_status = CONNECTING;
        vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 sec
        connection_status = CONNECTED;
        vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 sec
        connection_status = DISCONNECTED;
    }
}

void app_main(void)
{
    xTaskCreate(rgb_led_task, "rgb_led_task", 2048, NULL, 5, NULL);
    //xTaskCreate(connection_manager_task, "connection_manager_task", 2048, NULL, 5, NULL);

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    xTaskCreate(network_monitor_task, "network_monitor_task", 4096, NULL, 5, NULL);

    
}