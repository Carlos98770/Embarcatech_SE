#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "hardware/adc.h"


#include "ws2818b.pio.h"


// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7



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

void leds_intesity_index(uint index, uint r,uint g, uint b){

    if ( index >= 0 && index <= 24){
        uint r_i = (255*r)/100;
        uint g_i = (255*g)/100;
        uint b_i = (255*b)/100;

        npSetLED(index,r_i,g_i,b_i);
        
    }
}

void update_position(uint *pos_x_real,uint *pos_y_real, bool *flag,uint bar_pos_y,uint bar_pos_x ){

  if ( (*pos_x_real >= 0 && *pos_y_real >= 0 && *pos_x_real <= 4 && *pos_y_real <= 4)){
      
      if( bar_pos_x < 1 && *pos_x_real < 4 ){
        (*pos_x_real)++;
      }else if (bar_pos_x > 1 && *pos_x_real > 0){
        (*pos_x_real)--;
      }

      if (bar_pos_y > 1 && *pos_y_real < 4){
        (*pos_y_real)++;
      } else if (bar_pos_y < 1 && *pos_y_real > 0){
        (*pos_y_real)--;
      }

      if (*pos_x_real == 0 || *pos_x_real == 4 || *pos_y_real == 0 || *pos_y_real == 4 )
        *flag = true;
      else 
        *flag = false;

    }

}

int main() {

    // Inicializa entradas e saídas.
    stdio_init_all();
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);

    npInit(LED_PIN); //Inicializa a matriz de leds
    npClear();      //Limpar o buffer dos leds da matriz

    uint pos_x_real = 0;    //Inicializa a posicao_x no meio da matriz
    uint pos_y_real = 0;    //Inicializa a posicao_y no meio da matriz

    npSetLED(getIndex(pos_x_real,pos_y_real),255,0,0);
    npWrite();

    bool flag = false;
    
    while(true){
      
      adc_select_input(0);
      uint adc_y_raw = adc_read();
      adc_select_input(1);
      uint adc_x_raw = adc_read();

      //printf("ADC X: %d, ADC Y: %d\n", adc_x_raw, adc_y_raw);
      // Display the joystick position something like this:
      // X: [            o             ]  Y: [              o         ]
      const uint bar_width = 4;
      const uint adc_max = (1 << 12) - 2; //4094, resolução do conversor de 12 bits
      uint bar_x_pos = adc_x_raw * bar_width / adc_max;  
      uint bar_y_pos = adc_y_raw * bar_width / adc_max;

      printf("%d" ,bar_x_pos);
      

      update_position(&pos_x_real,&pos_y_real,&flag,bar_y_pos,bar_x_pos);

      npClear();
      
      npSetLED(getIndex(pos_x_real,pos_y_real),255,0,0);
      npWrite();
      

      //printf("POSIÇÃO X: %d ",pos_x_real);
      //printf("POSIÇÃO Y: %d\n",pos_y_real);

      sleep_ms(150);
    }
    

    return 0;
}