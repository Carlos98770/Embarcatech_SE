#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

const uint LED = 12;            // Pino do LED conectado
const uint16_t PERIOD = 2000;   // Período do PWM (valor máximo do contador)
const float DIVIDER_PWM = 16.0; // Divisor fracional do clock para o PWM
const uint16_t LED_STEP = 100;  // Passo de incremento/decremento para o duty cycle do LED
uint16_t led_level = 100;       // Nível inicial do PWM (duty cycle)


int BTN_SW_pwm = 22;
int BW_state_pwm = 0;

int WaitWithRead_pwm(int timeMS){                                     //Função para esperar um tempo e verificar se o botão foi pressionado
    for(int i = 0; i < timeMS; i = i+100){
        BW_state_pwm = !gpio_get(BTN_SW_pwm);
        if(BW_state_pwm == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}

void setup_pwm()
{
    uint slice;
    gpio_set_function(LED, GPIO_FUNC_PWM); // Configura o pino do LED para função PWM
    slice = pwm_gpio_to_slice_num(LED);    // Obtém o slice do PWM associado ao pino do LED
    pwm_set_clkdiv(slice, DIVIDER_PWM);    // Define o divisor de clock do PWM
    pwm_set_wrap(slice, PERIOD);           // Configura o valor máximo do contador (período do PWM)
    pwm_set_gpio_level(LED, led_level);    // Define o nível inicial do PWM para o pino do LED
    pwm_set_enabled(slice, true);          // Habilita o PWM no slice correspondente
}

void progama_3(){
    uint up_down = 1; // Variável para controlar se o nível do LED aumenta ou diminui
    stdio_init_all(); // Inicializa o sistema padrão de I/O
    setup_pwm();      // Configura o PWM

    while(true){
        pwm_set_gpio_level(LED, led_level); // Define o nível atual do PWM (duty cycle)
        WaitWithRead_pwm(1000);                 // Atraso de 1 segundo
        if(BW_state_pwm){
            led_level = 100;
            BW_state_pwm = 0;
            pwm_set_gpio_level(LED, 0);
            return;
        }
        if (up_down)
        {
            led_level += LED_STEP; // Incrementa o nível do LED
            if (led_level >= PERIOD)
                up_down = 0; // Muda direção para diminuir quando atingir o período máximo
        }
        else
        {
            led_level -= LED_STEP; // Decrementa o nível do LED
            if (led_level <= LED_STEP)
                up_down = 1; // Muda direção para aumentar quando atingir o mínimo
        }

        }
}