#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_oneshot.h"

#include "gpio_setup.h"
#include "nvs_handler.h"
#include "mqtt.h"

#define HALL_SENSOR_PIN 18
#define BUZZER 23

void read_hall_magnetic_sensor(void *pvParameter)
{
// Configura os pinos
pinMode(HALL_SENSOR_PIN, GPIO_INPUT);
pinMode(BUZZER, GPIO_OUTPUT);

// Variável de mensagem
char message[50];

while (1)
{
    // Lê o estado do sensor magnético Hall
    int sensorState = digitalRead(HALL_SENSOR_PIN);

    printf("Hall Magnetic Sensor: %d\n", sensorState);

    // Formata a mensagem para envio via MQTT
    sprintf(message, "{\"HallMagneticSensor\": %d}", sensorState);

    // Envia a mensagem via MQTT
    mqtt_send_message("v1/devices/me/telemetry", message);
    mqtt_send_message("v1/devices/me/attributes", message);

    // Grava o valor do sensor no armazenamento NVS
    save_nvs_value("HallMagneticSensor", sensorState);

    // Controla o buzzer com base no estado do sensor
    if (sensorState == 1)
    {
        gpio_set_level(BUZZER, 0);
    }
    else
    {
        gpio_set_level(BUZZER, 0);
        sleep(1);
        gpio_set_level(BUZZER, 1);
    }

    // Aguarda 1 segundo
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// Deleta a tarefa
vTaskDelete(NULL);

}
