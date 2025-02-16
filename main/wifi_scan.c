#include "wifi_scan.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

#define TAG "WiFi_Scan"
#define MAX_JSON_SIZE 2048  // Increased size to accommodate the new JSON structure

static char scan_results_json[MAX_JSON_SIZE]; // Buffer for JSON results

// Define 2.4 GHz and 5 GHz channels
static const uint8_t dual_band_channels[] = {
    // 2.4 GHz channels: 1-13 (commonly used)
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    // 5 GHz channels
    // UNII-1 (36-48)
    36, 40, 44, 48,
    // UNII-2 (52-64)
    52, 56, 60, 64,
    // UNII-2 Extended (100-144)
    100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144,
    // UNII-3 (149-165)
    149, 153, 157, 161, 165
};

// Helper function to get the security type from the encryption mode
const char* get_security_type(uint8_t encryption) {
    switch (encryption) {
        case WIFI_AUTH_OPEN:
            return "Open";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA2";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2";
        case WIFI_AUTH_WPA3_PSK:
            return "WPA3";
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA2/WPA3";
        default:
            return "UNKNOWN";
    }
}


const char* get_band(uint8_t channel) {
    if (channel >= 1 && channel <= 13) {
        return "2.4ghz";
    }
    if ((channel >= 36 && channel <= 144) || (channel >= 149 && channel <= 165)) {
        return "5ghz";
    }
    return "Unknown Band";
}

void wifi_scan() {
    ESP_LOGI(TAG, "Starting Dual-Band Wi-Fi Scan...");
    wifi_scan_config_t scan_config = {
        .ssid = NULL,       
        .bssid = NULL,    
        .channel = 0,        // 0 means scan all channels in the list
        .show_hidden = true  
    };

    snprintf(scan_results_json, sizeof(scan_results_json), "{\"rows\":[");

    for (size_t i = 0; i < sizeof(dual_band_channels) / sizeof(dual_band_channels[0]); i++) {
        scan_config.channel = dual_band_channels[i];
        esp_err_t err = esp_wifi_scan_start(&scan_config, true);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Wi-Fi scan failed on channel %d: %s", scan_config.channel, esp_err_to_name(err));
            snprintf(scan_results_json, sizeof(scan_results_json), "{\"rows\":[]}");
            return;
        }
        uint16_t ap_count = 0;
        esp_wifi_scan_get_ap_num(&ap_count);

        if (ap_count == 0) {
            ESP_LOGW(TAG, "No Wi-Fi networks found on channel %d.", scan_config.channel);
            continue;
        }
        wifi_ap_record_t *ap_records = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * ap_count);
        if (!ap_records) {
            ESP_LOGE(TAG, "Memory allocation failed!");
            snprintf(scan_results_json, sizeof(scan_results_json), "{\"rows\":[]}");
            return;
        }
        err = esp_wifi_scan_get_ap_records(&ap_count, ap_records);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get scan results on channel %d: %s", scan_config.channel, esp_err_to_name(err));
            free(ap_records);
            snprintf(scan_results_json, sizeof(scan_results_json), "{\"rows\":[]}");
            return;
        }
        size_t offset = strlen(scan_results_json); 
        for (int j = 0; j < ap_count; j++) {
            const char *security = get_security_type(ap_records[j].authmode);
            const char *band = get_band(scan_config.channel);
            char entry[512];  
            int len = snprintf(entry, sizeof(entry),
                               "{\"Channel\":\"%d\",\"MAC\":\"%02x:%02x:%02x:%02x:%02x:%02x\",\"SSID\":\"%s\",\"Security\":\"%s\",\"Band\":\"%s\"}%s",
                               scan_config.channel,
                               ap_records[j].bssid[0], ap_records[j].bssid[1], ap_records[j].bssid[2], ap_records[j].bssid[3], ap_records[j].bssid[4], ap_records[j].bssid[5],
                               ap_records[j].ssid, security, band,
                               (j == ap_count - 1) ? "" : ","); 

            if (offset + len < MAX_JSON_SIZE - 1) {
                snprintf(scan_results_json + offset, MAX_JSON_SIZE - offset, "%s", entry);
                offset += len;
            } else {
                ESP_LOGW(TAG, "Wi-Fi scan results truncated due to buffer size limit.");
                break;
            }
        }

        free(ap_records);
    }
    snprintf(scan_results_json + strlen(scan_results_json), MAX_JSON_SIZE - strlen(scan_results_json), "]}");
    scan_results_json[MAX_JSON_SIZE - 1] = '\0';

    ESP_LOGI(TAG, "Wi-Fi Scan Completed.");
}

const char* wifi_scan_get_results() {
    return scan_results_json; 
}
