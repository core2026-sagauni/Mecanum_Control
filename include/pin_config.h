#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include "driver/gpio.h"

#define DEFAULT_CAN_SPEED 1000000
#define MOTOR1_CAN_ID 0x1
#define MOTOR2_CAN_ID 0x2
#define MOTOR3_CAN_ID 0x3
#define MOTOR4_CAN_ID 0x4

// CAN ピン設定（ESP32の標準CANピン）
#define CAN_TX_GPIO GPIO_NUM_21
#define CAN_RX_GPIO GPIO_NUM_22

#endif