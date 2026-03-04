#ifndef CAN_H
#define CAN_H

#include "vehicle_msgs.h"

namespace can {
    void init();
    void send(const vehicle_msgs::can_msg &msg);
    void events();
}
#endif
