#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2818b.pio.h"
#include "hardware/adc.h"
#include "math.h"
#include "headers/interface.h"

//ARQUIVO RESPONSÁVEL PELA LOGICA FUNCIONAMENTO DO RASTREAMENTO SOLAR


// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7
#define LED_G 11

volatile float infos[4];


uint pos_x_real = 2;    //Inicializa a posicao_x no meio da matriz
uint pos_y_real = 2;    //Inicializa a posicao_y no meio da matriz
float acionamentos = 0;

int ponto_x = 2;
int ponto_y = 2;
float eficacia_porc;

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

//Inicializa o adc
void init_adc(){
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);
    
}

//Função responsavel pelo calculo da norma de um vetor
float norma(int v[], int n) {
    float soma = 0.0;
    for (int i = 0; i < n; i++) {
        soma += v[i] * v[i];
    }
    return sqrt(soma);
}

//Função responsavel pelo calculo do modulo de um numero
float abs_custom(float num) {
    if (num < 0) {
        return -num;  // Se o número for negativo, retornamos o seu oposto (positivo)
    }
    return num;  // Se o número for positivo ou zero, retornamos o próprio número
}

//Função responsavel pelo calculo da eficácia ou eficiência do sistema
float eficacia(int normal[], int vetor_luz[], int n) {
    double prod_escalar = 0.0;

    for (int i = 0; i < n; i++) {
        prod_escalar += normal[i] * vetor_luz[i];
    }

    double norma_normal = norma(normal, n);
    double norma_vetor_luz = norma(vetor_luz, n);

    if (norma_normal == 0 || norma_vetor_luz == 0) {
        //printf("Erro: Vetores com norma zero.\n");
        return 0;
    }
                                                                           //Soma de 0.000001 para evitar a divisao por 0     
    double cos_angulo = prod_escalar / ((norma_normal * norma_vetor_luz) + 0.000001);       //Coseno do angulo entre dois vetores

    // Evita erros numéricos (acos só aceita valores entre -1 e 1)
    if (cos_angulo < -1.0) cos_angulo = -1.0;
    if (cos_angulo > 1.0) cos_angulo = 1.0;

    double angulo = acos(cos_angulo);

    return abs_custom(cos(angulo) * 100.00); // Retorna a eficácia em porcentagem
}

//Função para atualização da posiçao no sistema cartesiano da matriz de leds, ou seja , X e [0,4] E y e [0,4]
void update_position(uint *pos_x_real,uint *pos_y_real,uint bar_pos_y,uint bar_pos_x ){

  if ( (*pos_x_real >= 0 && *pos_y_real >= 0 && *pos_x_real <= 4 && *pos_y_real <= 4)){
      
      if( bar_pos_x > 3000 && *pos_x_real > 0 ){
        (*pos_x_real)--;
      }else if (bar_pos_x < 700 && *pos_x_real < 4){
        (*pos_x_real)++;
      }

      if (bar_pos_y > 3000 && *pos_y_real < 4){
        (*pos_y_real)++;
      } else if (bar_pos_y < 700 && *pos_y_real > 0){
        (*pos_y_real)--;
      }

  }

}

//Função para ler o joystick e mudar sua posição
void ler_joystick(uint *pos_x_real,uint *pos_y_real){


    //printf("AGUARDANDO O MOVIMENTO DO JOYSTICK \n");
    adc_select_input(0);
    uint adc_y_raw = adc_read();
    adc_select_input(1);
    uint adc_x_raw = adc_read();

    update_position(pos_x_real,pos_y_real,adc_y_raw,adc_x_raw);
    //printf("POSIÇÃO X: %d || POSICÃO Y: %d\n",*pos_x_real,*pos_y_real);

    sleep_ms(10);
  

  //printf("\n\n");    
}

//Função responsável pelo armazenamento dos dados relevantes
void dados_para_thingspeaking(float dados[]){
    dados[0] = infos[0];
    dados[1] = infos[1];
    dados[2] = infos[2];
    dados[3] = infos[3];
}

//Função responsavel pelo inicio do sistema
void init_sistema(){
    npClear();      //Limpar o buffer dos leds da matriz
    npSetLED(getIndex(pos_x_real,pos_y_real),255,0,0);
    npWrite();
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
}


//Função responsavel pelo sistema
void sistema() {
  
    // Lê as coordenadas do joystick e armazena em pos_x_real e pos_y_real
    ler_joystick(&pos_x_real, &pos_y_real);

    // Define a posição do centro do painel solar
    int cord_centro[] = {2, 2, 0};

    // Define a posição da luz com base na leitura do joystick
    int cord_luz[] = {pos_x_real, pos_y_real, 1};

    // Define um ponto de referência para calcular a normal
    int ponto[] = {ponto_x, ponto_y, 1}; 

    int vetor_normal[3]; // Vetor normal ao painel
    int vetor_luz[3];    // Vetor direção da luz em relação ao painel

    // Calcula os vetores normal e direção da luz
    for (int i = 0; i < 3; i++) {
        vetor_normal[i] = ponto[i] - cord_centro[i];  // Vetor normal ao painel
        vetor_luz[i] = cord_centro[i] - cord_luz[i];  // Vetor da luz relativa ao centro
    }

    // Calcula a eficácia da orientação do painel com base nos vetores calculados
    eficacia_porc = eficacia(vetor_normal, vetor_luz, 3);

    // Atualiza informações sobre a posição e a eficácia do sistema
    infos[0] = (ponto_x - 2) * 15;  // Converte a posição X para uma escala apropriada
    infos[1] = (ponto_y - 2) * 15;  // Converte a posição Y para uma escala apropriada
    infos[2] = eficacia_porc;       // Armazena a eficácia calculada
    infos[3] = acionamentos;        // Número de acionamentos do motor

    // Verifica se a eficácia é maior ou igual a 75%
    if (eficacia_porc >= 75) {
        // Se a eficácia for alta, mantém o sistema como está e aguarda um curto tempo
        sleep_ms(100);
    } else {
        // Se a eficácia for baixa, ativa o motor para reposicionar o painel solar

        // Acende um LED para indicar que o motor está sendo acionado
        gpio_put(LED_G, 1);

        // Atualiza o status com as novas informações de posição e eficácia
        atualizar_status(eficacia_porc, (ponto_x - 2) * 15, (ponto_y - 2) * 15);

        // Atualiza a posição do painel para a nova posição sugerida pelo joystick
        ponto_x = pos_x_real;
        ponto_y = pos_y_real;
        
        // Incrementa o contador de acionamentos do motor
        acionamentos++;

        // Aguarda 2 segundos para permitir a movimentação do painel
        sleep_ms(2000);

        // Limpa a interface visual (provavelmente LEDs indicando a posição)
        npClear();

        // Atualiza a nova posição da placa com um LED vermelho indicando a nova posição
        npSetLED(getIndex(pos_x_real, pos_y_real), 255, 0, 0);
        npWrite();

        // Desliga o LED após a atualização da posição
        gpio_put(LED_G, 0);
    }
}
