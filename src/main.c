#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "modo_operacao.h"
#include "wifi.h"
#include "mqtt.h"
#include "nvs_handler.h"

SemaphoreHandle_t wifiConnectionSemaphore;
SemaphoreHandle_t mqttConnectionSemaphore;

void wifiConnected(void *params)
{
while (true)
{
if (xSemaphoreTake(wifiConnectionSemaphore, portMAX_DELAY))
{
// Processamento de conexão à Internet
mqtt_start();
}
}
vTaskDelete(NULL);
}

void handleServerCommunication(void *params)
{
#ifdef CONFIG_ENERGY_MODE
ESP_LOGI("MODE", "Energy Mode");
energy();
#else
ESP_LOGI("MODE", "Battery Mode");
battery();
#endif

vTaskDelete(NULL);

}

void app_main(void)
{
start_nvs();

wifiConnectionSemaphore = xSemaphoreCreateBinary();
mqttConnectionSemaphore = xSemaphoreCreateBinary();
wifi_start();

xTaskCreate(&wifiConnected, "MQTT Connection", 4096, NULL, 1, NULL);
xTaskCreate(&handleServerCommunication, "Server Communication", 4096, NULL, 1, NULL);

}