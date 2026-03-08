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
        uint8_t packet[UART_BUF_SIZE];
        int len = uart_read_bytes(UART_NUM, packet, UART_BUF_SIZE, pdMS_TO_TICKS(100));
        if (len > 0) {
            if (parsePacket(packet)) {
            } else {
                ESP_LOGW(TAG, "Invalid packet received");
            }            
        }
        return ;
    }
    // パケット解析
    bool parsePacket(const uint8_t* packet) {
        if (packet[0] != 0xAA || packet[1] != 0x55) {
            return false; // ヘッダが不正
        }
        vx = (packet[2] << 8) | packet[3];
        vy = (packet[4] << 8) | packet[5];
        v_rotation = (packet[6] << 8) | packet[7];
        vehicle::setMotorSpeeds(vx, vy, v_rotation);
        return true;
    }
} // namespace controller