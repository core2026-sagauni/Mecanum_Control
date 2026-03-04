#ifndef VEHICLE_H
#define VEHICLE_H
#include "vehicle_msgs.h"

namespace vehicle
{
    struct MecanumSpeeds {
        float frontLeft;   
        float frontRight;  
        float backLeft;    
        float backRight;   
    };

    // モーターインスタンス
    extern vehicle_msgs::vehicle_msg frontLeft;
    extern vehicle_msgs::vehicle_msg frontRight;
    extern vehicle_msgs::vehicle_msg backLeft;
    extern vehicle_msgs::vehicle_msg backRight;

    void init();
    void tick();

    // 速度を4つのモーター速度に変換する関数
    // vx: 前進/後進速度
    // vy: 左右移動速度
    // omega: 回転速度
    void setMotorSpeeds(float vx, float vy, float omega);
    void sendCanMessages();

} // namespace vehicle

#endif
