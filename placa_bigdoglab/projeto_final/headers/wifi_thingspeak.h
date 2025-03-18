#ifndef WIFI_THINGSPEAK_H
#define WIFI_THINGSPEAK_H

#include "lwip/tcp.h"

void enviar_thing_speak(float field1_value, float field2_value,float field3_value);
void conectar_wifi();

#endif // WIFI_THINGSPEAK_H