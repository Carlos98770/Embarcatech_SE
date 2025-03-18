
#include "headers/interface.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "headers/ssd1306.h"
#include "hardware/i2c.h"
#include <string.h>


//ARQUIVO RESPONSAVEL PELA INTERFACE DO SISTEMA, OU SEJA, A FORMATAÇÃO DAS MENSAGENS NO DISPLAY OLED

#define I2C_SDA 14
#define I2C_SCL 15

uint8_t ssd[ssd1306_buffer_length]; // Buffer de renderização para o display
char *texto[] = {
              "Iniciando",
              " Sistema",
              "Eficacia:  ",
              "Angulo X:    ",
              "Angulo Y:    "
};

int meiob;  
int meiof;

// Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};


void print_oled(char *text, uint8_t ssd[],uint8_t x, struct render_area *frame_area,int offsety,int offsetx){   //Função para printar mensagens no display

  int y = offsety;
  for (uint i = 0; i < x; i++)
  {
      ssd1306_draw_string(ssd, offsetx, y, text);
      y += 8;

  }
  render_on_display(ssd, frame_area);
  sleep_ms(10);

}

void limpar_oled(){                          //Função para limpar o display
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);
}


void init_I2c(){
   // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    limpar_oled();

}

  //Função responsavel pela atualização em tempo real das informações no OLED 
void atualizar_status(float eficacia, float angulo_x, float angulo_y){
  char eficacia_string[10];
  char angulo_x_string[10];
  char angulo_y_string[10];

  // Converte float para string
  sprintf(eficacia_string, "%.2f", eficacia); 
  sprintf(angulo_x_string, "%.2f", angulo_x); 
  sprintf(angulo_y_string, "%.2f", angulo_y); 

  print_oled(eficacia_string,ssd,1,&frame_area,8,(strlen(texto[2]))*8);
  print_oled(angulo_x_string,ssd,1,&frame_area,24,(strlen(texto[3]))*8);
  print_oled(angulo_y_string,ssd,1,&frame_area,40,(strlen(texto[4]))*8);
  
}

//Tela de inicialização do sistema
void imprimir_inicializacao(){

  meiob = 64 - (strlen(texto[0])*8)/2;
  meiof = 64 + (strlen(texto[0])*8)/2;
  print_oled(texto[0],ssd,1,&frame_area,16,meiob);
  print_oled(texto[1],ssd,1,&frame_area,32,meiob);
}

//Tela de impressão dos textos dos paramentros
void imprimir_status(){

   //tela dos paramentros
  print_oled(texto[2],ssd,1,&frame_area,8,0);
  print_oled(texto[3],ssd,1,&frame_area,24,0);
  print_oled(texto[4],ssd,1,&frame_area,40,0);
}

