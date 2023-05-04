//
// Created by Lay on 5/4/2023.
//
#include "esp_log.h"
#include "usr_sensors.h"
#include "bme280.h"

namespace usr {
    static sensor_humiture_handle_t humiture_handle;
    static bme280_handle_t pressure_handle;
    static sensor_light_handle_t light_handle;
    xQueueHandle sensor_data_queue;

    static esp_err_t sensors_init() {
        i2c_port_t i2c_port = 0;
        i2c_config_t conf{
                I2C_MODE_MASTER, 18, 17,
                GPIO_PULLUP_DISABLE, GPIO_PULLUP_DISABLE, {{100000}}, 0};
        auto i2c_bus = i2c_bus_create(i2c_port, &conf);
        humiture_handle = humiture_create(i2c_bus, SHT3X_ID);
        esp_err_t ret = humiture_test(humiture_handle);
        if (ret == ESP_FAIL) {
            ESP_LOGE("sensor", "humiture sensor init error!");
            return ESP_FAIL;
        }
        pressure_handle = bme280_create(i2c_bus, BME280_I2C_ADDRESS_DEFAULT);
        if (ret == ESP_FAIL) {
            ESP_LOGE("sensor", "pressure sensor init error!");
            return ESP_FAIL;
        }
        ret = bme280_default_init(pressure_handle);
        light_handle = light_sensor_create(i2c_bus, VEML6040_ID);
        ret = light_sensor_test(light_handle);
        if (ret == ESP_FAIL) {
            ESP_LOGE("sensor", "light sensor init error!");
            return ESP_FAIL;
        }
        return ESP_OK;
    }

    static void sensors_task(void *pvParameters) {
        TickType_t lastwaketime = xTaskGetTickCount();
        sensor_data_queue = xQueueCreate(10, sizeof(usr::sensor_data_t));
        float humidity, temperature, pressure, light_intensity;
        rgbw_t light_rgbw;
        uv_t light_uv;
        usr::sensor_data_t sensor_data;
        while (1) {
            esp_err_t ret = humiture_acquire_humidity(humiture_handle, &humidity);
            if (ret == ESP_FAIL) {
                ESP_LOGW("sensor", "humidity data access error!");
            }
            ret = humiture_acquire_temperature(humiture_handle, &temperature);
            if (ret == ESP_FAIL) {
                ESP_LOGW("sensor", "temperature data access error!");
            }
            ret = bme280_read_pressure(pressure_handle, &pressure);
            if (ret == ESP_FAIL) {
                ESP_LOGW("sensor", "pressure data access error!");
            }
            ret = light_sensor_acquire_light(light_handle, &light_intensity);
            light_sensor_acquire_rgbw(light_handle, &light_rgbw);
            light_sensor_acquire_uv(light_handle, &light_uv);
            if (ret == ESP_FAIL) {
                ESP_LOGW("sensor", "light data access error!");
            }
            sensor_data = {humidity, temperature, pressure, light_intensity, light_rgbw, light_uv};
            xQueueSend(sensor_data_queue, &sensor_data, 10 / portTICK_PERIOD_MS);
            vTaskDelayUntil(&lastwaketime, 1000 / portTICK_PERIOD_MS);
        }
    }

    esp_err_t sensors_task_create() {
        esp_err_t ret = sensors_init();
        if (ret == ESP_FAIL) {
            ESP_LOGE("sensor", "sensor task aborted");
            return ESP_FAIL;
        }
        xTaskCreate(sensors_task, "sensors", 2048, nullptr, 1, nullptr);
        return ESP_OK;
    }
}