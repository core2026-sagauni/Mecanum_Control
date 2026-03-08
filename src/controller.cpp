#include "controller.h"
#include "vehicle.h"

#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char* TAG = "CONTROLLER";

// UART設定
#define UART_NUM UART_NUM_1          // 使用するUARTポートによってこれ変えてください for リーくん
#define UART_RX_PIN GPIO_NUM_16
#define UART_TX_PIN GPIO_NUM_17
#define UART_BUF
#define UART_BAUD_RATE 115200
#define UART_BUF_SIZE (1024)

namespace controller
{ 
    int vx = 0;
    int vy = 0;
    int v_rotation = 0;

    // UART初期化
    void init() {
        // UART config
        uart_config_t uart_config = {
            .baud_rate = UART_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 122,
            .source_clk = UART_SCLK_DEFAULT,
        };

        // driver install and configuration
        ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
        // Set UART pins(TX, RX, RTS, CTS)
        ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
        ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0));
        xTaskCreate([](void*){
            while (true) {
                receive();
                vTaskDelay(10 / portTICK_PERIOD_MS); // 10msごとに受信チェック
            }
        }, "UARTReceiveTask", 4096, nullptr, 1, nullptr);
    }
    
    int send(const char* data)
    {
        const int len = strlen(data);
        const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
        return txBytes;
    }

    void receive(){
        uint8_t data[UART_BUF_SIZE];
        int len = uart_read_bytes(UART_NUM, data, UART_BUF_SIZE, pdMS_TO_TICKS(100));
        if (len > 0) {
            // Null終端を追加
            data[len] = '\0';
            if (parsePacket((char*)data, len)) {
                ESP_LOGI(TAG, "Parsed: vx=%d, vy=%d, v_rotation=%d", vx, vy, v_rotation);
            } else {
                ESP_LOGW(TAG, "Invalid packet received: %s", (char*)data);
            }            
        }
        return ;
    }
    
    // パケット解析 (例: "00,0001,E8,20,24,12")
    // フィールド2=vx, フィールド3=vy, フィールド4=v_rotation
    bool parsePacket(const char* packet, int len) {
        if (packet == nullptr || len == 0) {
            return false;
        }
        
        // データをコピーして処理
        char buffer[UART_BUF_SIZE];
        strncpy(buffer, packet, len);
        buffer[len] = '\0';
        
        // カンマ(とコロン)で分割してパース
        char* token;
        char* rest = buffer;
        int fieldIndex = 0;
        int parsedVx = 0, parsedVy = 0, parsedRotation = 0;
        
        while ((token = strtok_r(rest, ",:", &rest)) != nullptr) {
            switch (fieldIndex) {
                case 0: // ヘッダー (例: "00")
                    break;
                case 1: // ID等 (例: "0001")
                    break;
                case 2: // vx (例: "E8")
                    parsedVx = (int)strtol(token, nullptr, 16);
                    break;
                case 3: // vy (例: "20")
                    parsedVy = (int)strtol(token, nullptr, 16);
                    break;
                case 4: // v_rotation (例: "24")
                    parsedRotation = (int)strtol(token, nullptr, 16);
                    break;
                case 5: // チェックサム等 (例: "12")
                    break;
            }
            fieldIndex++;
        }
        
        if (fieldIndex >= 5) {
            vx = parsedVx;
            vy = parsedVy;
            v_rotation = parsedRotation;
            
            float norm_vx = (vx - 128) / 128.0f;
            float norm_vy = (vy - 128) / 128.0f;
            float norm_rotation = (v_rotation - 128) / 128.0f;
            
            vehicle::setMotorSpeeds(norm_vx, norm_vy, norm_rotation);
            return true;
        }
        
        return false;
    }
} // namespace controller