//
// Created by Lay on 5/4/2023.
//

#include "usr_gui.h"
#include "screen_driver.h"
#include "spi_bus.h"
#include "esp_check.h"
#include "lvgl_gui.h"

namespace usr {
    static esp_err_t screen_init(scr_driver_t *lcd) {
        spi_config_t spi_cfg = {
                GPIO_NUM_NC,
                (gpio_num_t) 11,
                (gpio_num_t) 12,
                4096
        };
        auto spi_bus = spi_bus_create(SPI2_HOST, &spi_cfg);

        scr_interface_spi_config_t lcd_if_cfg = {
                spi_bus,
                10,
                46,//TODO:
                20000000,
                false
        };
        scr_interface_driver_t *lcd_if_drv;
        scr_interface_create(SCREEN_IFACE_SPI, &lcd_if_cfg, &lcd_if_drv);
        ESP_RETURN_ON_ERROR(scr_find_driver(SCREEN_CONTROLLER_ST7789, lcd), "screen", "driver not found");
        scr_controller_config_t lcd_cfg = {
                lcd_if_drv, 3, -1, 0, 1, 0, 0, 240, 320, SCR_DIR_LRBT};
        ESP_RETURN_ON_ERROR((*lcd).init(&lcd_cfg), "screen", "screen init failed");
        return ESP_OK;
    }

    esp_err_t gui_task_create() {
        scr_driver_t lcd;
        ESP_RETURN_ON_ERROR(screen_init(&lcd), "gui", "screen init failed");
        ESP_RETURN_ON_ERROR(lvgl_init(&lcd, NULL), "gui", "lvgl init failed");
        return ESP_OK;
    }
}