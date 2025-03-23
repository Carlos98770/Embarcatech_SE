#include "headers/mqtt.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h"
#include "string.h"
#include "lwip/ip_addr.h"
#include "pico/cyw43_arch.h"


/*
    Falta Autorização a publicação de mensagens de Ips diferentes de Localhost
    Colocar autenticação no broker para a publicação e assinatura de mensagens

*/

// Callback para quando uma mensagem é recebida
static void call_back_connect(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if ( status == MQTT_CONNECT_ACCEPTED ) {
        printf("Conexão aceita!\n");
    } else {
        printf("Conexão recusada.\n");
    }
}

// Callback para quando uma mensagem é publicada
static void call_back_upload(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("Mensagem publicada com sucesso!\n");
    } else {
        printf("Erro ao publicar mensagem.\n");
    }
}

// Função principal
void mqtt_example_pub(char *topic, char *message) {
    // Configurações do cliente MQTT
    struct mqtt_connect_client_info_t mqtt_client_info = {
        .client_id = "meu_dispositivo", // ID do cliente
        .keep_alive = 60 ,               // Intervalo de keep-alive em segundos
        .client_user = "eduardo"
    };

    // Cria um novo cliente MQTT
    mqtt_client_t *client = mqtt_client_new();
    ip_addr_t broker_ip;
    if ( !ipaddr_aton("192.168.100.63",&broker_ip) ) {
        printf("Erro ao converter endereço IP.\n");
        return;
    }

    // Conecta ao broker Mosquitto em localhost
    err_t err = mqtt_client_connect(client, &broker_ip, 1883, call_back_connect, NULL, &mqtt_client_info);
    sleep_ms(1000);
    if (err == ERR_OK) {
        printf("Conectado ao broker Mosquitto em localhost!\n");
        // Publica uma mensagem no tópico "meu_topico"
        const char *topic_pub = topic;
        const char *message_pub = message;
        mqtt_publish(client, topic_pub, message_pub, strlen(message_pub), 1, 0, call_back_upload, NULL);
    } else {
        printf("Erro ao conectar ao broker.\n");
    }
}