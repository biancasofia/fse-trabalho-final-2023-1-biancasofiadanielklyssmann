#ifndef NVS_HANDLER_H
#define NVS_HANDLER_H

#include "esp_system.h"

void start_nvs();
int32_t le_valor_nvs(char *chave);
void save_nvs_value(char *chave, int32_t valor);

#endif