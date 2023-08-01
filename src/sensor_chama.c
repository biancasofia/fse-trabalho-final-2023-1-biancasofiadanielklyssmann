#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_oneshot.h"

#include "gpio_setup.c"
#include "nvs_handler.h"

#define FLAME_SENSOR_PIN 5

void read_flame_sensor(void *pvParameter)
{
// Configura o pino do sensor de chama
pinMode(FLAME_SENSOR_PIN, GPIO_INPUT);

// Variável de mensagem
char message[50];

while (1)
{
    // Lê o estado do sensor de chama
    int sensorState = digitalRead(FLAME_SENSOR_PIN);

    printf("Flame Sensor: %d\n", sensorState);

    // Formata a mensagem para envio via MQTT
    sprintf(message, "{\"FlameSensor\": %d}", sensorState);

    // Envia a mensagem via MQTT
    mqtt_send_message("v1/devices/me/telemetry", message);
    mqtt_send_message("v1/devices/me/attributes", message);

    // Grava o valor do sensor no armazenamento NVS
    save_nvs_value("FlameSensor", sensorState);

    // Aguarda 1 segundo
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// Deleta a tarefa
vTaskDelete(NULL);

}
