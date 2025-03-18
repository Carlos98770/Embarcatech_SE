#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "headers/ssd1306.h"
#include "hardware/i2c.h"
//#include "headers/pwm_led.h"
#include "hardware/pwm.h"
#include "headers/joystick_led.h"
//#include "headers/buzzer.h"

#define VRY 27         // Pino de leitura do eixo Y do joystick (conectado ao ADC)
#define ADC_CHANNEL_1 0 // Canal ADC para o eixo Y do joystick
#define SW 22         // Pino de leitura do botão do joystick
#define I2C_SDA 14
#define I2C_SCL 15
#define LED_R_PIN 12
#define LED_G_PIN 11
#define LED_B_PIN 13

uint state_SW = 0;
uint state_menu = 0;
uint8_t ssd[ssd1306_buffer_length]; // Buffer de renderização para o display
const char *texto[] = {"OPCAO 1","OPCAO 2","OPCAO 3","Joystick LED","BUZZER","LED RGB"};

// Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

void print_oled(const char *text, uint8_t ssd[],uint8_t x, struct render_area *frame_area,int offsety,int offsetx){   //Função para printar mensagens no display

  int y = offsety;
  for (uint i = 0; i < x; i++)
  {
      ssd1306_draw_string(ssd, offsetx, y, text);
      y += 8;

  }
  render_on_display(ssd, frame_area);
  sleep_ms(10);

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

void init_joystick(){
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);
  gpio_init(SW);
  gpio_set_dir(SW, GPIO_IN);
  gpio_pull_up(SW);   //nivel logico alto, sem estar pressionado
}

int len(const char *texto){
  int tam = 0;

  while(texto[tam] != '\0'){
    tam++;
  }

  return tam;
}


void opcoes(){
    
    // Espera o botão ser solto antes de entrar no loop principal
    while (!gpio_get(SW)){
    sleep_ms(50); // Debounce ao soltar
    }
    
    switch (state_menu) {
        case 0: progama_1(); break;
        //case 1: progama_2(); break;
        //case 2: progama_3(); break;
    }

    sleep_ms(10);
  
    // Aguarda o botão ser solto antes de retornar, evitando reentradas indesejadas
    while (!gpio_get(SW)){
      sleep_ms(50); // Debounce final
    }

    clean_oled();
    print_oled("VOLTANDO AO MENU",ssd,1,&frame_area,28,64 - (len("VOLTANDO AO MENU")*8)/2);
    sleep_ms(2000);
    clean_oled();
    sleep_ms(50);
    menu(texto);
 
}

void clean_led(){
  pwm_set_gpio_level(LED_R_PIN,0);
  
}

void menu(const char *texto[]) {
    int offset = 8; // Deslocamento inicial
    for (int i = 0; i < 3; i++) { // Percorre até encontrar NULL
        int posicao_y = (2 * i) * 8 + offset;
        int posicao_x = 38;

        print_oled(texto[i], ssd, 1, &frame_area, posicao_y, posicao_x);
    }
}

void linha_dinamica(int meiob, int meiof, int offset){
  ssd1306_draw_line(ssd,meiob,(2*state_menu + 1)*8 + offset,meiof,(2*state_menu + 1)*8 + offset,0);
    
  adc_select_input(ADC_CHANNEL_1);
  uint y_raw_adc = adc_read();
  if(!gpio_get(SW)){
    clean_oled();
    sleep_ms(50); //Tempo de debounce
    
    switch (state_menu) {
          case 0: print_oled(texto[3],ssd,1,&frame_area,28,64 - (len(texto[3])*8)/2); break;
          //case 1: print_oled(texto[4],ssd,1,&frame_area,28,64 - (len(texto[4])*8)/2); break;
          //case 2: print_oled(texto[5],ssd,1,&frame_area,28,64 - (len(texto[5])*8)/2); break;
      }
    
    opcoes();
  }

  if (y_raw_adc > 4000)
    state_menu = (state_menu + 2) % 3; // Equivalente a diminuir e circular
  else if (y_raw_adc < 100)
    state_menu = (state_menu + 1)%3; //AVANCA E CIRCULAR



  ssd1306_draw_line(ssd,meiob,(2*state_menu + 1)*8 + offset,meiof,(2*state_menu + 1)*8 + offset,1);

  render_on_display(ssd, &frame_area);
  sleep_ms(10);
}


int main() {
  stdio_init_all();
  init_joystick();
  init_I2c();
  ssd1306_init();  // Processo de inicialização completo do OLED SSD1306
  calculate_render_area_buffer_length(&frame_area);
  clean_oled();
  //pwm_init_buzzer();
  //setup_pwm();
  //clean_led();

  
  int meiob = 64 - (len(texto[0])*8)/2;
  int meiof = 64 + (len(texto[0])*8)/2;
  int offset = 8;
  menu(texto);
  

  while (true) {
    
    linha_dinamica(meiob,meiof,offset);
    sleep_ms(100);
  }

  return 0;
}
