#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "pico/cyw43_arch.h"
#include "headers/mqtt.h"

#define wifi_ssid "MEDEIROS SILVA"
#define wifi_password "lage1890"

//Documentação MQTT lwip https://www.nongnu.org/lwip/2_1_x/group__mqtt.html

void conect_wifi(){
    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    //printf("Conectando ao Wi-Fi...\n");
    int flag = 1;
    while(flag){
        printf("Tentando Conectar ao Wi-Fi...\n");
        if (cyw43_arch_wifi_connect_timeout_ms(wifi_ssid, wifi_password, CYW43_AUTH_WPA2_AES_PSK, 5000)) {
            printf("Falha ao conectar ao Wi-Fi\n");
            
        } else {
            printf("Conectado ao Wi-Fi!\n");
            // Exibe o endereço IP
            printf("Endereço IP: %s\n", ip4addr_ntoa(&cyw43_state.netif[0].ip_addr));
            flag = 0;
        }
    }

    flag = 1;
}




int main()
{
    stdio_init_all();
    sleep_ms(10000);
    // Initialise the Wi-Fi chip
    conect_wifi();

    while (true) {
        float temp = 25.0;
        char *topic = "Temperatura";
        char str_temp[20];  // Buffer para armazenar a string
    
        sprintf(str_temp, "%.2f", temp);  // Converte float para string com 2 casas decimais
        sleep_ms(1000);
        mqtt_example_pub(topic, str_temp);
        sleep_ms(10000);
        temp++;
    }
}

