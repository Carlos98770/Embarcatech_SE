#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7
#define BUTTON_A_PIN 6


// Definição de pixel GRB
struct pixel_t {
 uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

/**
* Inicializa a máquina PIO para controle da matriz de LEDs.
*/
void npInit(uint pin) {

 // Cria programa PIO.
 uint offset = pio_add_program(pio0, &ws2818b_program);
 np_pio = pio0;

 // Toma posse de uma máquina PIO.
 sm = pio_claim_unused_sm(np_pio, false);
 if (sm < 0) {
   np_pio = pio1;
   sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
 }

 // Inicia programa na máquina PIO obtida.
 ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

 // Limpa buffer de pixels.
 for (uint i = 0; i < LED_COUNT; ++i) {
   leds[i].R = 0;
   leds[i].G = 0;
   leds[i].B = 0;
 }
}

/**
* Atribui uma cor RGB a um LED.
*/
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

/**
* Limpa o buffer de pixels.
*/
void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

/**
* Escreve os dados do buffer nos LEDs.
*/
void npWrite() {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
}

/**
 * Converte as coordenadas (x, y) na matriz 5x5 para o índice da fila linear.
 * 
 * @param x A coluna (0 a 4).
 * @param y A linha (0 a 4).
 * @return O índice correspondente na fila (0 a 24).
 */
int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return y * 5 + x; // Linha par (esquerda para direita).
    } else {
        return y * 5 + (4 - x); // Linha ímpar (direita para esquerda).
    }
}

void leds_intesity(int x, int y, int r,int g, int b){

    if ( (x >=0 && x <=4) && ( y >=0 && y <=4)){
        uint r_i = (255*r)/100;
        uint g_i = (255*g)/100;
        uint b_i = (255*b)/100;

        uint index = getIndex(x,y);
        npSetLED(index,r_i,g_i,b_i);
        npWrite();
    }
}


void buttom_init(){
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
}

int main() {

    // Inicializa entradas e saídas.
    stdio_init_all();
    buttom_init();
    // Inicializa matriz de LEDs NeoPixel.
    npInit(LED_PIN);
    bool flag = false;
    // Não faz mais nada. Loop infinito.
    while (true) {
        
        if(gpio_get(BUTTON_A_PIN) == 0){
            if (!flag){

                flag = true;
                
                npSetLED(2,0,1,0);
                npSetLED(6,0,1,0);
                npSetLED(8,0,1,0);
                npSetLED(10,0,1,0);
                npSetLED(14,0,1,0);
                npSetLED(15,0,1,0);
                npSetLED(17,0,1,0);
                npSetLED(19,0,1,0);
                npSetLED(21,0,1,0);
                npSetLED(23,0,1,0);



                npWrite(); // Escreve os dados nos LEDs.
            }
        } else if (flag){
            npClear();
            npWrite();
            flag = false;
        }
        sleep_ms(10);
    }

    return 0;
}