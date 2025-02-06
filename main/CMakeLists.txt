idf_component_register(SRCS "main.c" "connect.c" "blink.c" "monitor.c"
                    INCLUDE_DIRS "."
                    REQUIRES led_strip nvs_flash esp_netif esp_wifi esp_event lwip)