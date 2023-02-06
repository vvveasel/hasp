/* MIT License - Copyright (c) 2019-2022 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_ETHERNET_ESP32_H
#define HASP_ETHERNET_ESP32_H

#include "ArduinoJson.h"

void ethernetSetup();
void ethernetLoop(void);

bool ethernetEverySecond();
bool ethernetEvery5Seconds();
void ethernet_get_statusupdate(char* buffer, size_t len);
void ethernet_get_ipaddress(char* buffer, size_t len);

void ethernet_get_info(JsonDocument& doc);

#endif