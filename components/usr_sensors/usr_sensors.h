
//
// Created by Lay on 5/4/2023.
//

#ifndef SILLY_IOT_FARM_USR_SENSORS_H
#define SILLY_IOT_FARM_USR_SENSORS_H

#include "iot_sensor_hub.h"

namespace usr {
    struct sensor_data_t {
        float humidity;
        float temperature;
        float pressure;
        float light_intensity;
        rgbw_t light_rgbw;
        uv_t light_uv;
    };
    extern xQueueHandle sensor_data_queue;
    esp_err_t sensors_task_create();
}
#endif //SILLY_IOT_FARM_USR_SENSORS_H
