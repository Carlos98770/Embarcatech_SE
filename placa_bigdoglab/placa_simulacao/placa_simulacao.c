#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2818b.pio.h"
#include "hardware/adc.h"
#include "math.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7


uint pos_x_real = 2;    //Inicializa a posicao_x no meio da matriz
uint pos_y_real = 2;    //Inicializa a posicao_y no meio da matriz
int acionamentos = 0;

int ponto_x = 2;
int ponto_y = 2;

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
void init_adc(){
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);
}

float norma(int v[], int n) {
    float soma = 0.0;
    for (int i = 0; i < n; i++) {
        soma += v[i] * v[i];
    }
    return sqrt(soma);
}

float eficacia(int normal[], int vetor_luz[], int n) {
    double prod_escalar = 0.0;

    for (int i = 0; i < n; i++) {
        prod_escalar += normal[i] * vetor_luz[i];
    }

    double norma_normal = norma(normal, n);
    double norma_vetor_luz = norma(vetor_luz, n);

    if (norma_normal == 0 || norma_vetor_luz == 0) {
        printf("Erro: Vetores com norma zero.\n");
        return 0;
    }

    double cos_angulo = prod_escalar / ((norma_normal * norma_vetor_luz) + 0.000001);

    // Evita erros numéricos (acos só aceita valores entre -1 e 1)
    if (cos_angulo < -1.0) cos_angulo = -1.0;
    if (cos_angulo > 1.0) cos_angulo = 1.0;

    double angulo = acos(cos_angulo);

    return cos(angulo) * 100; // Retorna a eficácia em porcentagem
}

void update_position(uint *pos_x_real,uint *pos_y_real,uint bar_pos_y,uint bar_pos_x ){

  if ( (*pos_x_real >= 0 && *pos_y_real >= 0 && *pos_x_real <= 4 && *pos_y_real <= 4)){
      
      if( bar_pos_x > 4000 && *pos_x_real > 0 ){
        (*pos_x_real)--;
      }else if (bar_pos_x < 100 && *pos_x_real < 4){
        (*pos_x_real)++;
      }

      if (bar_pos_y > 4000 && *pos_y_real < 4){
        (*pos_y_real)++;
      } else if (bar_pos_y < 1000 && *pos_y_real > 0){
        (*pos_y_real)--;
      }

  }

}

void ler_joystick(uint *pos_x_real,uint *pos_y_real){


    //printf("AGUARDANDO O MOVIMENTO DO JOYSTICK \n");
    adc_select_input(0);
    uint adc_y_raw = adc_read();
    adc_select_input(1);
    uint adc_x_raw = adc_read();

    update_position(pos_x_real,pos_y_real,adc_y_raw,adc_x_raw);
    //printf("POSIÇÃO X: %d || POSICÃO Y: %d\n",*pos_x_real,*pos_y_real);

    sleep_ms(100);
  

  printf("\n\n");    
}

void sistema(){
  npClear();      //Limpar o buffer dos leds da matriz
  npSetLED(getIndex(pos_x_real,pos_y_real),255,0,0);
  npWrite();

  while(true){
    ler_joystick(&pos_x_real,&pos_y_real);

    
    int cord_centro[] = {2, 2, 0};
    int cord_luz[] = {pos_x_real, pos_y_real, 1};
    int ponto[] = {ponto_x, ponto_y, 1}; // Adicionando um ponto para calcular a normal

    int vetor_normal[3];
    int vetor_luz[3];

    for (int i = 0; i < 3; i++) {
        vetor_normal[i] = ponto[i] - cord_centro[i];  // Definição correta da normal
        vetor_luz[i] = cord_luz[i] - cord_centro[i];  // Vetor luz relativo ao centro
    }

    float eficacia_porc = eficacia(vetor_normal, vetor_luz, 3);

    printf("Posição angular da placa no eixo X : %d° || ", (ponto_x - 2) * 5);
    printf("Posição angular da placa no eixo Y: %d° || ", (ponto_y - 2) * 5);
    printf("EFICACIA: %.2f%% || ", eficacia_porc);
    printf("ACIONAMENTOS DO MOTOR: %d\n",acionamentos);

    
    if (eficacia_porc >= 75) {
        printf("EFICACIA MAIOR QUE 75\n");
        sleep_ms(100);
    } else {
        printf("EFICACIA MENOR OU IGUAL A 75\n");
        printf("ACIONANDO O MOTOR PARA ROTAÇÃO DO PAINEL\n");
        sleep_ms(3000);
        ponto_x =  pos_x_real;
        ponto_y = pos_y_real;
        npClear();

        printf("NOVA POSIÇÃO DA PLACA\n");
        sleep_ms(1000); 
        npSetLED(getIndex(pos_x_real,pos_y_real),255,0,0);
        npWrite();
        acionamentos++;
    }

    
  }
}

int main() {

    // Inicializa entradas e saídas.
    stdio_init_all();
    init_adc();
    npInit(LED_PIN); //Inicializa a matriz de leds
    sistema();



    return 0;
}