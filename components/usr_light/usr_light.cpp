//
// Created by Lay on 5/4/2023.
//


#include "usr_light.h"
#include "esp_log.h"
#include "esp_check.h"


namespace usr {
    xQueueHandle light_queue = nullptr;

    static void light_task(void *pvParameters) {
        color_t color{};
        led_state new_state{};
        while (1) {
            xQueueReceive(light_queue, &color, portMAX_DELAY);
            uint32_t color_value = color.red << 16 | color.green << 8 | color.blue;
            for (auto &&i: new_state.leds) {
                i = color_value;
            }
            ESP_ERROR_CHECK(ws2812_write_leds(new_state));
        }
    }

    esp_err_t light_task_create() {
        ESP_RETURN_ON_ERROR(ws2812_control_init(), "light", "led init error,task aborted");
        light_queue = xQueueCreate(1, sizeof(color_t));
        xTaskCreate(light_task, "light task", 1024, nullptr, 2, nullptr);
        return ESP_OK;
    }

}