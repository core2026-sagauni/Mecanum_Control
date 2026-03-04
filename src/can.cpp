#include <can.h>
#include "driver/twai.h"
#include "pin_config.h"
#include "esp_log.h"

static const char* TAG = "CAN";

namespace can {    
    
    void init() {
        // TWAI（CAN）の一般設定
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_GPIO, CAN_RX_GPIO, TWAI_MODE_NORMAL);
        
        // タイミング設定（1Mbps）
        twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
        
        // フィルタ設定（全メッセージ受信）
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
        
        // TWAIドライバのインストール
        if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
            ESP_LOGI(TAG, "TWAI driver installed");
        } else {
            ESP_LOGE(TAG, "Failed to install TWAI driver");
            return;
        }
        
        // TWAIドライバの起動
        if (twai_start() == ESP_OK) {
            ESP_LOGI(TAG, "TWAI driver started");
        } else {
            ESP_LOGE(TAG, "Failed to start TWAI driver");
        }
    }

    void send(const vehicle_msgs::can_msg &msg) {
        twai_message_t message = {};
        message.identifier = msg.id;
        message.data_length_code = msg.len;
        for (int i = 0; i < msg.len; i++) {
            message.data[i] = msg.data[i];
        }
        message.flags = 0;  // 標準フレーム
        
        // メッセージ送信（タイムアウト10ms）
        if (twai_transmit(&message, pdMS_TO_TICKS(10)) == ESP_OK) {
            ESP_LOGD(TAG, "Message queued for transmission");
        } else {
            ESP_LOGW(TAG, "Failed to queue message for transmission");
        }
    }
    
    void events() {
        // メッセージ受信処理
        twai_message_t message;
        while (twai_receive(&message, 0) == ESP_OK) {
            ESP_LOGD(TAG, "Received message ID: 0x%lx, DLC: %d", message.identifier, message.data_length_code);
            // 受信したメッセージの処理をここに追加
        }
    }

}