#include "i2c_bus.h"
#include <iostream>
#include "usr_sensors.h"
#include "usr_motor.h"
#include "usr_light.h"
#include "usr_gui.h"

extern "C" void app_main(void) {
    usr::sensors_task_create();
    usr::motor_task_create();
    usr::light_task_create();
    usr::gui_task_create();
}
