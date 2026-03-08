#include "vehicle.h"
#include "can.h"
#include "pin_config.h"
#include "controller.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern "C" void app_main() {
  can::init();
  vehicle::init();
  controller::init();
  
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}