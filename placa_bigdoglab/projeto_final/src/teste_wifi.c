#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "lwip/apps/http_client.h"
#include "lwip/dns.h"
#include "headers/wifi_thingspeak.h"

/*
                        ATENÇÃO ---> CASO NAO SEJA POSSIVEL CONECTAR O WIFI, NAO SERÁ POSSIVEL O ENVIO DAS REQUISIÇÕES HTTP PARA O THINGSPEAK
*/

#define WIFI_SSID " "    // <------            //AQUI, COLOQUE O ID DO WIFI
#define WIFI_PASS " "    // <------            //AQUI, COLOQUE A SENHA DO WIFI
#define THINGSPEAK_API_KEY "94KACL0ZI12HO6QZ"   //API KEY DA MINHA API CRIADA
#define THINGSPEAK_HOST "api.thingspeak.com"    

// Função de callback para resolução DNS
static void dns_resolve_callback(const char *name, const ip_addr_t *ipaddr, void *arg) {
    if (ipaddr != NULL) {
        printf("DNS resolvido: %s -> %s\n", name, ipaddr_ntoa(ipaddr));
        *((ip_addr_t *)arg) = *ipaddr; // Armazena o endereço IP resolvido
    } else {
        printf("Falha ao resolver DNS para: %s\n", name);
    }
}

// Função de callback para receber dados do servidor
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    if (p != NULL) {
        printf("Resposta do servidor:\n%.*s\n", p->len, (char *)p->payload);
        pbuf_free(p); // Libera o buffer
    } else if (err == ERR_OK) {
        tcp_close(pcb); // Fecha a conexão
        printf("Conexão fechada pelo servidor.\n");
    }
    return ERR_OK;
}

void enviar_thing_speak(float field1_value, float field2_value, float field3_value) {
    // Formata os dados no formato de corpo da requisição POST
    char dados_posts[256];
    snprintf(dados_posts, sizeof(dados_posts), "api_key=%s&field1=%.2f&field2=%.2f&field3=%.2f",
             THINGSPEAK_API_KEY, field1_value, field2_value, field3_value);

    printf("Enviando dados para ThingSpeak: %s\n", dados_posts);

    // Usando IP fixo para evitar resolução DNS
    ip_addr_t server_ip;
    IP4_ADDR(&server_ip, 184, 106, 153, 149); // IP do ThingSpeak

    // Cria uma estrutura para o cliente HTTP
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    // Configura o callback para receber dados
    tcp_recv(pcb, tcp_recv_callback);

    // Conecta ao servidor ThingSpeak na porta 80 (HTTP)
    err_t err = tcp_connect(pcb, &server_ip, 80, NULL);
    if (err != ERR_OK) {
        printf("Erro ao conectar ao servidor: %d\n", err);
        tcp_abort(pcb); // Aborta a conexão em caso de erro
        return;
    }
    printf("Conexão TCP estabelecida com sucesso!\n");

    // Prepara a requisição HTTP POST
    char requisicao[512];
    int tamanho = snprintf(requisicao, sizeof(requisicao),
                   "POST /update HTTP/1.1\r\n"
                   "Host: %s\r\n"
                   "Content-Type: application/x-www-form-urlencoded\r\n"
                   "Content-Length: %d\r\n"
                   "Connection: close\r\n"
                   "\r\n"
                   "%s", THINGSPEAK_HOST, strlen(dados_posts), dados_posts);

    printf("Requisição HTTP:\n%s\n", requisicao);

    // Envia a requisição HTTP
    err = tcp_write(pcb, requisicao, tamanho, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        printf("Erro ao enviar requisição HTTP: %d\n", err);
        tcp_abort(pcb); // Aborta a conexão em caso de erro
        return;
    }
    printf("Requisição HTTP enviada com sucesso!\n");

    // Aguarda a resposta do servidor com timeout reduzido (1 segundo)
    absolute_time_t tempo_resposta = make_timeout_time_ms(1000);
    while (absolute_time_diff_us(get_absolute_time(), tempo_resposta) > 0) {
        sleep_ms(100);
    }

    // Finaliza a conexão
    tcp_close(pcb);
    printf("Conexão TCP fechada.\n");

    printf("Dados enviados com sucesso para ThingSpeak\n");
}

void conectar_wifi(){
    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 1000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return;
    } else {
        printf("Conectado ao Wi-Fi!\n");
        // Exibe o endereço IP
        printf("Endereço IP: %s\n", ip4addr_ntoa(&cyw43_state.netif[0].ip_addr));
    }
}
