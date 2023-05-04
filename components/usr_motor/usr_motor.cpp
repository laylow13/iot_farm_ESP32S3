//
// Created by Lay on 5/4/2023.
//

#include "usr_motor.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"


#define GPIO_MOTOR_SW 42

namespace usr {

    SemaphoreHandle_t motor_semaphr = nullptr;

    static void motor_task(void *pvParameters) {
        while (1) {
            xSemaphoreTake(motor_semaphr, portMAX_DELAY);
            auto current_level = gpio_get_level((gpio_num_t) GPIO_MOTOR_SW);
            esp_err_t ret = gpio_set_level((gpio_num_t) GPIO_MOTOR_SW, current_level == 0 ? 1 : 0);
            if (ret == ESP_FAIL)
                ESP_LOGW("motor", "motor switch error");
        }
    }

    esp_err_t motor_task_create() {
        gpio_config_t config{GPIO_MOTOR_SW, GPIO_MODE_OUTPUT,
                             GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE,
                             GPIO_INTR_DISABLE};
        gpio_config(&config);
        esp_err_t ret = gpio_set_level((gpio_num_t) GPIO_MOTOR_SW, 0);
        motor_semaphr = xSemaphoreCreateBinary();
        xTaskCreate(motor_task, "motor", 1024, nullptr, 3, nullptr);
        if (ret == ESP_FAIL) {
            ESP_LOGW("motor", "motor switch error");
            return ESP_FAIL;
        }
        return ESP_OK;
    }
}