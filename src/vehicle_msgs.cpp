#include "vehicle_msgs.h"
#include <cstring>

namespace vehicle_msgs {
    vehicle_msg::vehicle_msg() {
        std::memset(velocity, 0, sizeof(velocity));
    }

    void vehicle_msg::to_can_msg(can_msg &msg) {
        msg.id = motor_id_;
        msg.len = 8;
        msg.data[0] = velocity[0];
        msg.data[1] = velocity[1];
        msg.data[2] = velocity[2];
        msg.data[3] = velocity[3];
        for (int i = 4; i < 8; i++) {
            msg.data[i] = 0;
        }
    }

    void vehicle_msg::update(const float vel) {
        std::memcpy(velocity, &vel, sizeof(float));
    }

    void vehicle_msg::set_motor_id(const uint8_t id) {
        motor_id_ = id + 0x200;
    }
}
