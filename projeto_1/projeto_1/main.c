#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Definindo os pinos dos LEDs
#define LEDR 0   // LED vermelho
#define LEDY 5   // LED amarelo
#define LEDG 6   // LED verde
#define LEDG_PED 8  //LED verde do Pedestre
#define PUSH_BUTTOM 15 //Botão do Pedestre
#define BUZZER_PIN 11  // Configuração do pino do buzzer
#define BUZZER_FREQUENCY 2000 // Configuração da frequência do buzzer (em Hz)

volatile int button_value = 0;   //Define o estado volatil do botão, ou seja, o botão pode mudar de valor a qualquer hora
#define TIMER_GREEN 8000        //Duração do estado verde no semáforo em sua rotina normal
#define TIMER_YELLOW 2000       //Duração do estado amarelo no semáforo em sua rotina normal
#define TIMER_RED 10000       //Duração do estado vermelho no semáforo em sua rotina normal

  /*
      Essa função é responsavel pelo acionamentos dos leds
  */
void led_signal_put(bool r, bool y, bool g, bool gp) {
    gpio_put(LEDR, r);   // Acende ou apaga o LED vermelho
    gpio_put(LEDY, y);   // Acende ou apaga o LED amarelo
    gpio_put(LEDG, g);   // Acende ou apaga o LED verde
    gpio_put(LEDG_PED, gp);//LED PEDESTRE
}
  /*
      Essa função é responsavel pela inicialização do pino do push_buttom
  */
void init_buttom(){
  gpio_init(PUSH_BUTTOM);  // Inicializa o pino GPIO
  gpio_set_dir(PUSH_BUTTOM, GPIO_IN);  // Define o pino como entrada
  gpio_pull_up(PUSH_BUTTOM);  // Habilita o resistor de pull-down interno (botão normalmente fechado)
}
/*
      Essa função é responsavel pela inicialização dos pinos do leds
  */
void init_leds() {
  //Inicializa os pinos dos leds usados
    gpio_init(LEDR);      
    gpio_init(LEDY);
    gpio_init(LEDG);
    gpio_init(LEDG_PED);

    gpio_set_slew_rate(LEDR, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDY, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDG, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDG_PED, GPIO_SLEW_RATE_SLOW);

    gpio_set_dir(LEDR, true);  // Configura o pino do LED como saída
    gpio_set_dir(LEDY, true);  // Configura o pino do LED como saída
    gpio_set_dir(LEDG, true);  // Configura o pino do LED como saída
    gpio_set_dir(LEDG_PED, true);  // Configura o pino do LED como saída

}
/*
      Essa função é responsavel pela inicialização do pino do buzzer e configurações do proprio
  */
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}
/*
      Essa função é responsavel pela alteração do valor que guarda o estado do botão
  */
void pressed_buttom(){
    button_value = 1;
}
/*
    Definição de uma função para emitir um beep quando o o tempo do sinal verde do pedestre estiver a 4s de acabar
*/
void beep(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);
    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 1024);
    // Temporização
    sleep_ms(duration_ms);
    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);
    // Pausa entre os beeps
    sleep_ms(100); // Pausa de 100ms
}
/*
   Essa função defini a rotina quando o botão é pressionado, deixando o semáforo no sinal amarelo
   e apos algum tempo, o sinal verde para os pedestre será ligado juntamente com o buzzer para as pessoas com deficiencia auditiva
   Com o tempo restante de 4s, o buzzer começar a bipar, indicando que o sinal irá abrir para os carros.
*/
void pedestrian_sign(){
    uint64_t last_change_time = to_ms_since_boot(get_absolute_time()); // Obtém o tempo atual em milissegundos desde a inicialização do sistema
    char state = 'Y';       //Estado atual do semáforo
    uint64_t duration = TIMER_YELLOW + 3000;   //Duração do estado amarelo + 3s
    led_signal_put(false,true,false,false);    //Acende o sinal amarelo

    while (true) {
        uint64_t now = to_ms_since_boot(get_absolute_time());  // Obtém o tempo atual em milissegundos desde a inicialização do sistema

        // Verifica se a duração foi atingida
        if (now - last_change_time >= duration) {
            // Altera o estado e a duração dependendo do estado atual
            if (state == 'Y') {  // Amarelo
                led_signal_put(true,false,false,true);  //Acende o vermelho pros carros e verde para os pedestre
                state = ' '; //Define um estado invalido

                // Obter o slice do PWM associado ao pino
                uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

                // Configurar o duty cycle para 50% (ativo)
                pwm_set_gpio_level(BUZZER_PIN, 1024);

                duration = TIMER_RED + 5000;  // Duração do amarelo

            } else {
                button_value = 0;       //Reserta o valor do push_buttom para 0
                pwm_set_gpio_level(BUZZER_PIN, 0);    //Faz o buzzer parar de emitir som
                traffic_signal_control();             //Chama a função para a rotina normal do semáforo
            }
            // Atualiza o tempo de mudança
            last_change_time = now;
        }

        if (now - last_change_time >= duration - 4000 && state != 'Y') {
            beep(BUZZER_PIN,200);   //Faltando 4s para o sinal verde do pedestre fecha, chama a funcao beep
        }

        // Aguarda 100ms para evitar saturar a saída
        sleep_ms(100);
    }
}
/*
  A função que controla o semáfora na sua rotina norma, caso seja registrado o aperto do botão a função irá
  chama a função que cuida dessa rotina especifica que é a pedestrian_sign()
*/
void traffic_signal_control() {
    
    uint64_t last_change_time = to_ms_since_boot(get_absolute_time()); // Obtém o tempo atual em milissegundos desde a inicialização do sistema
    char state = 'G';   //Define o estado atual
    uint64_t duration = TIMER_GREEN;   //Define o tempo de duração
    led_signal_put(false,false,true,false);

    //Indentifica o aperto do botão e chama a funcao pressed_buttom, assim alterando o valor do buttom_value.
    gpio_set_irq_enabled_with_callback(PUSH_BUTTOM, GPIO_IRQ_EDGE_FALL, true, &pressed_buttom);

    while (!button_value) {
        uint64_t now = to_ms_since_boot(get_absolute_time()); // Obtém o tempo atual em milissegundos desde a inicialização do sistema

        // Verifica se a duração foi atingida
        if (now - last_change_time >= duration) {
            // Altera o estado e a duração dependendo do estado atual
            if (state == 'G') {  // Verde
                led_signal_put(false,true,false,false);
                state = 'Y';  // Muda para amarelo
                duration = TIMER_YELLOW;  // Duração do amarelo
            } else if (state == 'Y') {  // Amarelo
                led_signal_put(true,false,false,false);
                state = 'R';  // Muda para vermelho
                duration = TIMER_RED;  // Duração do vermelho
            } else if (state == 'R') {  // Vermelho
                led_signal_put(false,false,true,false);
                state = 'G';  // Muda para verde
                duration = TIMER_GREEN;  // Duração do verde
            }

            // Atualiza o tempo de mudança
            last_change_time = now;
        }
        // Aguarda 100ms para evitar saturar a saída
        sleep_ms(100);
    }

    pedestrian_sign(); //Chama a função da rotina quando o botão for apertado
}

int main() {
    stdio_init_all();  // Inicializa a comunicação serial
   

    init_buttom();  //Inicializa o push_buttom
    init_leds();  // Inicializa os LEDs
    pwm_init_buzzer(BUZZER_PIN);  //Inicializa o buzzer
    traffic_signal_control();  // Inicia o controle do semáforo
    
    return 0;
}
