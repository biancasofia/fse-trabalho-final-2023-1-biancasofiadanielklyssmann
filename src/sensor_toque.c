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

#define TOUCH_PIN 4  // pino de entrada do sensor de toque
#define PIN_RED    25 // GIOP23
#define PIN_GREEN  33 // GIOP22
#define PIN_BLUE   26 // GIOP21

extern void mqtt_send_message(char *topic, char *message);

void read_touch_sensor()
{
// Configura os pinos
pinMode(TOUCH_PIN, GPIO_INPUT);
pinMode(PIN_RED, GPIO_OUTPUT);
pinMode(PIN_GREEN, GPIO_OUTPUT);
pinMode(PIN_BLUE, GPIO_OUTPUT);

// Variável de mensagem
char message[50];

// Variáveis de controle dos LEDs
bool ledOn = false;
bool ledOff = true;

while (1)
{
    // Lê o estado do sensor de toque
    int sensorState = digitalRead(TOUCH_PIN);

    printf("Touch Sensor: %d\n", sensorState);

    // Formata a mensagem para envio via MQTT
    sprintf(message, "{\"TouchSensor\": %d}", sensorState);

    // Envia a mensagem via MQTT
    mqtt_send_message("v1/devices/me/telemetry", message);
    mqtt_send_message("v1/devices/me/attributes", message);

    // Grava o valor do sensor no armazenamento NVS
    save_nvs_value("TouchSensor", sensorState);

    // Controla os LEDs com base no estado do sensor
    if (sensorState == 1 && ledOff == true)
    {
        gpio_set_level(PIN_RED, 1);
        gpio_set_level(PIN_BLUE, 0);
        gpio_set_level(PIN_GREEN, 0);
        ledOff = false;
        ledOn = true;
    }
    else if (sensorState == 1)
    {
        gpio_set_level(PIN_RED, 0);
        gpio_set_level(PIN_BLUE, 1);
        gpio_set_level(PIN_GREEN, 1);
        ledOff = true;
        ledOn = false;
    }

    // Aguarda 1 segundo
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// Deleta a tarefa
vTaskDelete(NULL);

}