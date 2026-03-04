#ifndef VEHICLE_MSGS_H
#define VEHICLE_MSGS_H

#include <cstdint>

namespace vehicle_msgs {
    struct can_msg {
        uint32_t id;
        uint8_t len = 8;
        uint8_t data[8];
    };

    class vehicle_msg{
        public:
            vehicle_msg();
            ~vehicle_msg() = default;
            void to_can_msg(can_msg &msg);
            void update(const float vel);
            void set_motor_id(const uint8_t id);
            uint8_t velocity[4];
            uint32_t motor_id_ = 0;
    };
}
#endif
