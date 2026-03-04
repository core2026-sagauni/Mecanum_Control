#include "vehicle.h"
#include "can.h"
#include "pin_config.h"
#include <algorithm>
#include <cmath>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace vehicle
{
    // グローバルインスタンスの実装
    vehicle_msgs::vehicle_msg frontLeft;
    vehicle_msgs::vehicle_msg frontRight;
    vehicle_msgs::vehicle_msg backLeft;
    vehicle_msgs::vehicle_msg backRight;

    void init(){
        frontLeft.set_motor_id(MOTOR1_CAN_ID);
        frontRight.set_motor_id(MOTOR2_CAN_ID);
        backLeft.set_motor_id(MOTOR3_CAN_ID);
        backRight.set_motor_id(MOTOR4_CAN_ID);
        xTaskCreate([](void*){
            while (true) {
                tick();
                vTaskDelay(10 / portTICK_PERIOD_MS); // 10msごとに更新
            }
        }, "VehicleTask", 4096, nullptr, 1, nullptr);
    }

    void tick(){
        // ここでセンサーからの入力を処理して、vx, vy, omegaを計算する
        float vx = 0.0f;    // 前進/後進速度
        float vy = 0.0f;    // 左右移動速度
        float omega = 0.0f; // 回転速度
        setMotorSpeeds(vx, vy, omega);
        sendCanMessages();
    }
    
    void setMotorSpeeds(float vx, float vy, float omega) {
        MecanumSpeeds speeds;
        
        // mecanum wheel speed calculation
        speeds.frontLeft  = vx - vy - omega;
        speeds.frontRight = vx + vy + omega;
        speeds.backLeft   = vx + vy - omega;
        speeds.backRight  = vx - vy + omega;
        
        // normalization (limit maximum speed to 1.0)
        float maxSpeed = std::max({
            std::abs(speeds.frontLeft),
            std::abs(speeds.frontRight),
            std::abs(speeds.backLeft),
            std::abs(speeds.backRight)
        });
        
        if (maxSpeed > 1.5f) {
            float ration = 1.5f / maxSpeed; 
            speeds.frontLeft  = speeds.frontLeft * ration;
            speeds.frontRight = speeds.frontRight * ration;
            speeds.backLeft   = speeds.backLeft * ration;
            speeds.backRight  = speeds.backRight * ration;
        }

        frontLeft.update(speeds.frontLeft);
        frontRight.update(speeds.frontRight);
        backLeft.update(speeds.backLeft);
        backRight.update(speeds.backRight);
    }

    void sendCanMessages() {
        vehicle_msgs::can_msg msg;
        frontLeft.to_can_msg(msg);
        can::send(msg);
        frontRight.to_can_msg(msg);
        can::send(msg);
        backLeft.to_can_msg(msg);
        can::send(msg);
        backRight.to_can_msg(msg);
        can::send(msg);
    }
} // namespace vehicle
