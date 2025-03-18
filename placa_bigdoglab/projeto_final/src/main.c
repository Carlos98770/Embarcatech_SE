#include "pico/stdlib.h"
#include <stdio.h>
#include "headers/wifi_thingspeak.h"
#include "pico/cyw43_arch.h"
#include "headers/placa_simulacao.h"
#include "hardware/timer.h"
#include "headers/interface.h"

/*

                    ATENÇÃO --> PARA O FUNCIONAMENTO DO MODULO WIFI E CONSEQUENTEMENTE A REQUISIÇÕES HTTP
                    É NECESSARIO COLOCAR O WIFI ID E SENHA NO ARQUIVO teste_wifi.c NAS DEFINIÇÕES WIFI_SSID e WIFI_PASS

*/

#define pin 7           //Pino da Matriz de LEDs

int flag_dados  = 0;       //Flag para o envio de dados
int flag_atualizar = 0;    //Flag para atualizar os dados no display OLED

float dados[4];            //Vetor que terá as informações para serem mostradas no OLED

//Função de callback para a flag de dados
bool enviar_dados_callback(struct repeating_timer *t) {
    flag_dados = 1;
    return true;
}

//Função de callback para a flag de atualização
bool atualizar_dados_callback(struct repeating_timer *t){
    flag_atualizar = 1;
    return true;
}

int main() {
    stdio_init_all();  // Inicializa a saída padrão
    hardware_alarm_claim(0); // Reserva o timer 0 para uso
    init_I2c();
    
    conectar_wifi();    // Conecta ao Wi-Fi
    npInit(pin);       // Inicializa o LED (ou outro dispositivo)
    init_adc();        // Inicializa o ADC

    // Configura o timer para enviar dados a cada 30 segundos    
    struct repeating_timer timer_dados;
    add_repeating_timer_ms(30000, enviar_dados_callback, NULL, &timer_dados);

    // Configura outro timer para atualização dos dados a cada 2 segundos 
    struct repeating_timer atualizar_dados;
    add_repeating_timer_ms(2000, atualizar_dados_callback, NULL, &atualizar_dados);
    

    imprimir_inicializacao();   //Imprimi a mensagem de inicialização no OLED
    sleep_ms(1800);
    init_sistema();    // Inicializa o sistema
    limpar_oled();

    imprimir_status();  //Imprimi o texto das variaveis

    while (true) {

       
        sistema();      //Função do sistema de monitoramento solar
        dados_para_thingspeaking(dados);    //Coleta as informações do sistema

        if (flag_dados){
            enviar_thing_speak(dados[0], dados[1], dados[2]);       //Envia os dados para thigSpeak se a flag_dados = 1 
            flag_dados = 0;
        }

        if (flag_atualizar){                                        //Atualiza os dados do OLED caso flag_atualizar = 1
            atualizar_status(dados[2], dados[0], dados[1]);
            flag_atualizar = 1;
        }
    

        sleep_ms(100);
        

    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}