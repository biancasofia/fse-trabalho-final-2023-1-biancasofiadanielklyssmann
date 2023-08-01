#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "dht11.h"
#include "pwm.h"
#include "sensor_toque.h"
#include "sensor_magnetico.c"
#include "sensor_chama.c"
#include "esp_sleep.h"
#include "esp32/rom/uart.h"
#include <time.h>
#include "driver/rtc_io.h"
#include <stdlib.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_oneshot.h"
#include "gpio_setup.h"
#include "nvs_handler.h"
#include "mqtt.h"
#include "wifi.h"

#define HALL 4

void energy()
{
config_pwm();

xTaskCreate(&init_dht11, "Temperature_Humidity", 2048, NULL, 1, NULL);
xTaskCreate(&read_touch_sensor, "Touch_Sensor_Task", 2048, NULL, 1, NULL);
xTaskCreate(&read_hall_magnetic_sensor, "Hall_Magnetic_Sensor_Task", 2048, NULL, 10, NULL);
xTaskCreate(read_flame_sensor, "Flame_Sensor_Task", 2048, NULL, 10, NULL);

}

void battery()
{
char message[50];

// Configuração da GPIO para o botão de entrada
pinMode(HALL, GPIO_INPUT_PULLUP);

gpio_wakeup_enable(HALL, GPIO_INTR_HIGH_LEVEL);
esp_sleep_enable_gpio_wakeup();

while (true)
{
    printf("Light Sleep Mode...\n");
    esp_light_sleep_start();
    // Configura o modo sleep somente após completar a escrita na UART para finalizar o printf
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    int state = digitalRead(HALL); // lê o estado do pino

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_GPIO)
    {
        printf("\nO sensor foi ativado!\n");
        printf("Hall: %d\n", state);
        sprintf(message, "{\"Hall\": %d}", state);
        mqtt_send_message("v1/devices/me/telemetry", message);
        mqtt_send_message("v1/devices/me/attributes", message);
        save_nvs_value("Hall", state);
        cause = ESP_SLEEP_WAKEUP_TIMER;
        printf("Mensagem enviada!\n\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS); // aguarde por 1 segundo
    }
}

}
