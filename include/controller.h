#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

namespace controller
{
    void init();
    void receive();
    bool parsePacket(const uint8_t* packet);
    int send(const char* data);
    
} // namespace controller
#endif // CONTROLLER_H
