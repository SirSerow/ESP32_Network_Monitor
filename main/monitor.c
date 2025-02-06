#include <stdio.h>
#include <string.h>
#include <inttypes.h> // For PRIu32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_ping.h"
#include "ping/ping_sock.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#define PING_TARGET "8.8.8.8"  // Google DNS for latency measurement
#define MONITOR_INTERVAL_MS 5000  // Monitor every 5 seconds

static const char *TAG = "NetworkMonitor";

// Structure to hold network parameters
typedef struct {
    int8_t rssi;          // WiFi signal strength (RSSI in dBm)
    uint32_t latency;     // Latency (in ms)
    uint32_t jitter;      // Jitter (in ms)
    uint32_t throughput;  // Throughput (kbps)
    bool ofdma_enabled;   // OFDMA status
    bool twt_enabled;     // TWT status
    bool rssi_received;
    bool latency_received;
    bool throughput_received;
    bool ofdma_received;
    bool twt_received;
} network_params_t;

network_params_t net_params = {0};

// Function to measure WiFi RSSI
void measure_rssi() {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        net_params.rssi = ap_info.rssi;
        net_params.rssi_received = true;
        ESP_LOGI(TAG, "RSSI: %d dBm", net_params.rssi);
    } else {
        ESP_LOGW(TAG, "Failed to get RSSI");
    }
}

// Function to check OFDMA support
void measure_ofdma() {
    wifi_sta_list_t sta_list;
    if (esp_wifi_ap_get_sta_list(&sta_list) == ESP_OK) {
        net_params.ofdma_enabled = (sta_list.num > 0);  // If multiple devices, OFDMA likely in use
        net_params.ofdma_received = true;
        ESP_LOGI(TAG, "OFDMA Enabled: %s", net_params.ofdma_enabled ? "Yes" : "No");
    } else {
        ESP_LOGW(TAG, "Failed to get OFDMA status");
    }
}

// Function to check TWT (Target Wake Time) Efficiency
void measure_twt() {
    wifi_config_t config;
    if (esp_wifi_get_config(WIFI_IF_STA, &config) == ESP_OK) {
        net_params.twt_enabled = config.sta.listen_interval > 0;  // TWT is enabled if listen_interval is set
        net_params.twt_received = true;
        ESP_LOGI(TAG, "TWT Enabled: %s", net_params.twt_enabled ? "Yes" : "No");
    } else {
        ESP_LOGW(TAG, "Failed to get TWT status");
    }
}

// **Ping Callbacks**
static void ping_on_success(esp_ping_handle_t hdl, void *args) {
    uint32_t elapsed_time;
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));

    net_params.latency = elapsed_time;
    net_params.latency_received = true;

    ESP_LOGI(TAG, "Latency: %" PRIu32 " ms", net_params.latency);
}

static void ping_on_timeout(esp_ping_handle_t hdl, void *args) {
    ESP_LOGW(TAG, "Ping Timeout");
}


static void ping_on_end(esp_ping_handle_t hdl, void *args) {
    uint32_t transmitted, received, total_time_ms;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));

    // Corrected format specifier for uint32_t
    ESP_LOGI("PING", "%" PRIu32 " packets transmitted, %" PRIu32 " received, time %" PRIu32 " ms",
             transmitted, received, total_time_ms);
}

void measure_latency() {
    ip_addr_t target_addr;
    struct addrinfo hint, *res = NULL;
    
    memset(&hint, 0, sizeof(hint));
    memset(&target_addr, 0, sizeof(target_addr));

    // Resolve domain to IP
    if (getaddrinfo(PING_TARGET, NULL, &hint, &res) != 0) {
        ESP_LOGE(TAG, "Failed to resolve hostname: %s", PING_TARGET);
        return;
    }

    struct in_addr addr4 = ((struct sockaddr_in *)(res->ai_addr))->sin_addr;
    inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
    freeaddrinfo(res);

    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ping_config.target_addr = target_addr;
    ping_config.count = 4;
    ping_config.interval_ms = 1000;

    esp_ping_callbacks_t cbs = {
        .on_ping_success = ping_on_success,
        .on_ping_timeout = ping_on_timeout,
        .on_ping_end = ping_on_end,
        .cb_args = NULL
    };

    esp_ping_handle_t ping;
    esp_err_t err = esp_ping_new_session(&ping_config, &cbs, &ping);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create ping session: %s", esp_err_to_name(err));
        return;
    }

    err = esp_ping_start(ping);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start ping session: %s", esp_err_to_name(err));
        return;
    }
}


// Function to check if all parameters are collected
void check_and_print_results() {
    if (net_params.rssi_received && net_params.latency_received &&
        net_params.throughput_received && net_params.ofdma_received &&
        net_params.twt_received) {
        ESP_LOGI(TAG, "Network Parameters Collected:");
        ESP_LOGI(TAG, "RSSI: %d dBm, Latency: %" PRIu32 " ms, Throughput: %" PRIu32 " kbps, OFDMA: %s, TWT: %s",
                 net_params.rssi, net_params.latency, net_params.throughput,
                 net_params.ofdma_enabled ? "Enabled" : "Disabled",
                 net_params.twt_enabled ? "Enabled" : "Disabled");
        net_params.rssi_received = false;
        net_params.latency_received = false;
        net_params.throughput_received = false;
        net_params.ofdma_received = false;
        net_params.twt_received = false;
    }
}

// **Network monitor task**
void network_monitor_task(void *arg) {
    while (1) {
        measure_rssi();
        measure_ofdma();
        measure_twt();
        measure_latency();
        vTaskDelay(pdMS_TO_TICKS(MONITOR_INTERVAL_MS));
        check_and_print_results();
    }
}

