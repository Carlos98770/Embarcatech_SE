#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

//O seguinte progama implementa um semaforo para pedestre com os leds vermelho, amarelo e verde, e um botão para simular a ação de um pedestre.
//O programa é dividido em 3 etapas: Sinal Fechado, Sinal de Atenção e Sinal Aberto.
//O sinal fechado dura 8s, caso o botão seja pressionado, o sinal de atenção é acionado por 5s e depois o sinal aberto por 15s.
//Caso o botão não seja pressionado, o sinal de atenção é acionado por 2s e depois o sinal aberto por 10s.
//O programa é repetido indefinidamente.


// This example will use I2C1 on GPIO14 (SDA) and GPIO15 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments

#define I2C_SDA 14
#define I2C_SCL 15
#define LED_R_PIN 13                    //Aqui definimos os pinos dos leds e do botão
#define LED_G_PIN 11
#define LED_B_PIN 12
#define BTN_A_PIN 5




uint8_t ssd[ssd1306_buffer_length]; // Buffer de renderização para o display
int A_state = 0;    //Botao A está pressionado?

char *text_vermelho[] = {           //Mensagens para o display no sinal fechado
    " SINAL FECHADO  ",
    " ",
    "    AGUARDE      "
  };

char *text_amarelo [] = {           //Mensagens para o display no sinal de atenção
  "     SINAL ",
  "      DE",
  "    ATENCAO",
  " ",
  "  PREPARE-SE "
};
char *text_verde [] = {             //Mensagens para o display no sinal abertoS
  " SINAL ABERTO",
  " ",
  "  ATRAVESSAR ",
  "  COM CUIDADO "
};

// Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

void print_oled(char *text[], uint8_t ssd[],uint8_t x, struct render_area *frame_area){   //Função para printar mensagens no display

  int y = 0;
  for (uint i = 0; i < x; i++)
  {
      ssd1306_draw_string(ssd, 5, y, text[i]);
      y += 8;
  }
  render_on_display(ssd, frame_area);

}
                                          //Funções para acionar os leds e printar mensagens no display
void SinalAberto(){                       
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0); 
    print_oled(text_verde,ssd,count_of(text_verde),&frame_area); 
}

void SinalAtencao(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);
    print_oled(text_amarelo,ssd,count_of(text_amarelo),&frame_area);
}

void SinalFechado(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
    print_oled(text_vermelho,ssd,count_of(text_vermelho),&frame_area);
}

int WaitWithRead(int timeMS){                                     //Função para esperar um tempo e verificar se o botão foi pressionado
    for(int i = 0; i < timeMS; i = i+100){
        A_state = !gpio_get(BTN_A_PIN);
        if(A_state == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}

void init_Leds(){

  // INICIANDO LEDS
  gpio_init(LED_R_PIN);
  gpio_set_dir(LED_R_PIN, GPIO_OUT);
  gpio_init(LED_G_PIN);
  gpio_set_dir(LED_G_PIN, GPIO_OUT);
  gpio_init(LED_B_PIN);
  gpio_set_dir(LED_B_PIN, GPIO_OUT);

}

void init_Buttom(){
  // INICIANDO BOTÄO
  gpio_init(BTN_A_PIN);
  gpio_set_dir(BTN_A_PIN, GPIO_IN);
  gpio_pull_up(BTN_A_PIN);   //nivel logico alto, sem estar pressionado

}

void init_I2c(){
   // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);


}

void clean_oled(){                          //Função para limpar o display
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);
}

int main(){
  stdio_init_all();
  init_Leds();
  init_Buttom();
  init_I2c();

  ssd1306_init();  // Processo de inicialização completo do OLED SSD1306
  calculate_render_area_buffer_length(&frame_area);

  while(true){
    
      clean_oled();
      SinalFechado();
      A_state = WaitWithRead(8000);   //espera com leitura do botäo
      //sleep_ms(8000);

      if(A_state){               //ALGUEM APERTOU O BOTAO - SAI DO SEMAFORO NORMAL
          //SINAL AMARELO POR 5s && Mensagem de atenção para os Pedestres
          clean_oled();
          SinalAtencao();     
          sleep_ms(5000);

          //SINAL VERMELHO PARA OS CARROS POR 15s && Verde para os Pedestre && mensagem para atravessar
          clean_oled();
          SinalAberto();
          sleep_ms(15000);

      }else{                          //NINGUEM APERTOU O BOTAO - CONTINUA NO SEMAFORO NORMAL
          clean_oled();              
          SinalAtencao(); //SINAL AMARELO POR 2s && Mensagem de atenção para os Pedestres
          sleep_ms(2000);

          //SINAL VERMELHO PARA OS CARROS POR 10s && Verde para os Pedestres
          clean_oled();
          SinalAberto();
          sleep_ms(10000);
      }
              
  }

  return 0;

}