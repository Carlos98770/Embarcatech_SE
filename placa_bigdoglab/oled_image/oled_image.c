#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

void init_I2c(){
   // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void print_oled(char *text[], uint8_t ssd[],uint8_t x, struct render_area *frame_area){

  int y = 0;
  for (uint i = 0; i < x; i++)
  {
      ssd1306_draw_string(ssd, 5, y, text[i]);
      y += 8;
  }
  render_on_display(ssd, frame_area);

}

// Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

int main()
{
    stdio_init_all();   // Inicializa os tipos stdio padrão presentes ligados ao binário
    init_I2c();
  
   
    ssd1306_init();  // Processo de inicialização completo do OLED SSD1306

    calculate_render_area_buffer_length(&frame_area);

    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

//restart:

// Parte do código para exibir a mensagem no display (opcional: mudar ssd1306_height para 32 em ssd1306_i2c.h)
// /**
    char *text[] = {
        "  Carlos   ",
        "  Eduardo  ",
        "  Medeiros ",
        "  Da Silva "};
    uint8_t x = count_of(text);
    //print_oled(text,ssd,x,&frame_area);
  
// */

// Parte do código para exibir a linha no display (algoritmo de Bresenham)
/**
    ssd1306_draw_line(ssd, 10, 10, 100, 50, true);
    render_on_display(ssd, &frame_area);
*/

// Parte do código para exibir o bitmap no display

    const uint8_t bitmap_128x64[] = {
	0xff, 0x7f, 0xfe, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 
	0xff, 0x3f, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 
	0xff, 0x9f, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 
	0xff, 0xe7, 0xff, 0xff, 0xff, 0xe3, 0xff, 0x7f, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xe3, 0xff, 0x3f, 
	0xff, 0xf3, 0xff, 0xff, 0xff, 0xe3, 0xff, 0x3f, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xe1, 0xff, 0x3f, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0x1f, 0x00, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0x1f, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0x1f, 0x00, 
	0xff, 0xc7, 0xff, 0xff, 0xff, 0xe1, 0x1e, 0x00, 0x3f, 0x80, 0xff, 0xff, 0xff, 0xe1, 0x1f, 0x00, 
	0x00, 0x80, 0xff, 0xff, 0xff, 0xe1, 0x1f, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xe1, 0x1f, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xe1, 0x1f, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xe1, 0x3f, 0x80, 
	0x00, 0x00, 0xfe, 0xff, 0xff, 0xe1, 0x1f, 0xfc, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xe1, 0x17, 0x7e, 
	0x00, 0x00, 0xfc, 0xff, 0xff, 0xe0, 0x27, 0x3f, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xe0, 0x07, 0x3e, 
	0x00, 0x00, 0xf8, 0xff, 0xff, 0xe0, 0x07, 0x1e, 0x00, 0x00, 0xf8, 0xff, 0x7f, 0xe0, 0xcf, 0x0e, 
	0x00, 0x00, 0xf8, 0xff, 0x7f, 0xe0, 0x5f, 0x05, 0x00, 0x00, 0xf8, 0xff, 0x7f, 0xe0, 0x9f, 0x01, 
	0x00, 0x00, 0xf0, 0xff, 0x3f, 0xe0, 0x1f, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x3f, 0xe0, 0x3f, 0x00, 
	0x00, 0x20, 0xf0, 0xff, 0x3f, 0xe0, 0x7f, 0x00, 0x00, 0x20, 0xe0, 0xff, 0x1f, 0xc0, 0xff, 0x00, 
	0x00, 0x20, 0xc0, 0xff, 0x1f, 0x80, 0xff, 0x00, 0x00, 0x20, 0xc0, 0xff, 0x1f, 0x80, 0x3f, 0x00, 
	0x00, 0x02, 0x80, 0xff, 0x0f, 0x00, 0x0f, 0x01, 0x00, 0x06, 0x80, 0xff, 0x0f, 0x00, 0xbe, 0x00, 
	0x00, 0x04, 0x80, 0xff, 0x07, 0x00, 0x7c, 0x00, 0x00, 0x04, 0x80, 0xff, 0x07, 0x00, 0x3c, 0x00, 
	0x00, 0x04, 0xc0, 0xff, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x01, 0x00, 0x08, 0x00, 
	0x10, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x30, 0x00, 0xe0, 0xff, 0x02, 0x00, 0x00, 0x00, 
	0x30, 0x00, 0xe0, 0x7f, 0x03, 0x00, 0x00, 0x00, 0x30, 0x00, 0xe0, 0xff, 0x03, 0x00, 0x00, 0x00, 
	0x20, 0x00, 0xe0, 0xbf, 0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0xe0, 0xdf, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xf0, 0xef, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xf0, 0xf7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xfb, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xf8, 0xff, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfd, 0x00, 0xfe, 0x00, 0x00, 
	0x00, 0x00, 0xfe, 0xfe, 0x00, 0xfe, 0x01, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x00, 0xfe, 0x01, 0x00, 
	0x00, 0x00, 0x70, 0x7f, 0x00, 0xfe, 0x01, 0x00, 0x00, 0x00, 0xb0, 0x7f, 0x00, 0xfe, 0x01, 0x00, 
	0x00, 0x00, 0x80, 0x7f, 0x00, 0xfe, 0x03, 0x00, 0x00, 0x80, 0xc0, 0x7f, 0x00, 0xfe, 0x03, 0x00, 
	0x00, 0x80, 0xc0, 0x7f, 0x00, 0xfe, 0x03, 0x00, 0x00, 0xc0, 0x80, 0x7f, 0x80, 0xfe, 0x03, 0xc0, 
	0x00, 0xc0, 0x80, 0x7f, 0x80, 0xfc, 0x03, 0xf0, 0x00, 0xc0, 0xc0, 0x7f, 0x80, 0xfe, 0x01, 0xf8, 
	0x00, 0xe0, 0xc0, 0x7f, 0xe0, 0x7e, 0x01, 0xfc, 0x00, 0xe0, 0xc0, 0x7f, 0x60, 0x7e, 0x00, 0xfe, 
	0x00, 0x60, 0xc0, 0x7f, 0x70, 0x7f, 0x00, 0xff, 0x00, 0x60, 0xc0, 0x7b, 0x70, 0x5f, 0x80, 0xff, 
	0x00, 0x70, 0xe0, 0x73, 0xf0, 0x1f, 0xe0, 0xff, 0x00, 0x70, 0xe0, 0x71, 0xf0, 0xcb, 0xf0, 0xff, 
	0x00, 0x70, 0x60, 0x70, 0xf0, 0x61, 0xf8, 0xff, 0x00, 0x30, 0x20, 0xf0, 0xf0, 0x71, 0xfc, 0xff, 
	0x00, 0x38, 0x10, 0xf8, 0xf0, 0x79, 0xfc, 0xff, 0x00, 0x38, 0x00, 0xf0, 0xf0, 0x71, 0xfe, 0xff, 
	0x00, 0x38, 0x00, 0xf0, 0xe1, 0x31, 0xfe, 0xff, 0x00, 0x1c, 0x00, 0xf0, 0xe1, 0x01, 0xff, 0xff, 
	0x00, 0x1c, 0x00, 0xf0, 0xe1, 0x01, 0xff, 0xff, 0x00, 0x1c, 0x00, 0xf0, 0xc1, 0x00, 0xff, 0xff, 
	0x00, 0x1c, 0x00, 0xf0, 0x03, 0x80, 0xff, 0xff, 0x00, 0x0e, 0x00, 0xf8, 0x03, 0x80, 0xff, 0xff, 
	0x00, 0x0e, 0x00, 0xf8, 0x03, 0x80, 0xff, 0xff, 0x00, 0x0e, 0x00, 0xf8, 0x07, 0xc0, 0xff, 0xff, 
	0x00, 0x07, 0x00, 0xf8, 0x07, 0xc0, 0xff, 0xff, 0x00, 0x07, 0x00, 0xfa, 0x0f, 0xc2, 0xff, 0xff, 
	0x00, 0x07, 0x00, 0xc8, 0x0f, 0xc3, 0xff, 0xff, 0x00, 0x07, 0x00, 0x0d, 0x0f, 0xe3, 0xff, 0xff, 
	0x80, 0x07, 0x00, 0x07, 0x01, 0xe3, 0xff, 0xff, 0x80, 0x03, 0x00, 0x0f, 0x07, 0xf1, 0xff, 0xff, 
	0x80, 0x03, 0x00, 0x0f, 0x07, 0xf0, 0xff, 0xff, 0xc0, 0x03, 0x00, 0x0f, 0x07, 0xf0, 0xff, 0xff, 
	0xc0, 0x01, 0x00, 0x0f, 0x07, 0xf0, 0xff, 0xff, 0xc0, 0x01, 0x00, 0x8f, 0x03, 0xfa, 0xff, 0xff, 
	0xc0, 0x01, 0x80, 0x8f, 0x01, 0xfc, 0xff, 0xff, 0xe0, 0x00, 0x80, 0x8f, 0x01, 0xfe, 0xff, 0xff, 
	0xe0, 0x00, 0x80, 0xcf, 0x01, 0xec, 0xff, 0xff, 0xa0, 0x00, 0x80, 0xf7, 0x03, 0xec, 0xff, 0xff, 
	0xb0, 0x00, 0xc0, 0xf7, 0x01, 0xe8, 0xff, 0xff, 0x50, 0x00, 0xc0, 0xe7, 0x01, 0xf8, 0xff, 0xff, 
	0x70, 0x00, 0xc0, 0xe7, 0x01, 0xf0, 0xff, 0xff, 0x30, 0x00, 0xc0, 0xe7, 0x01, 0xf0, 0xff, 0xff, 
	0x28, 0x00, 0xe0, 0xe7, 0x01, 0xe0, 0xff, 0xff, 0x08, 0x00, 0xe0, 0xe3, 0x11, 0xe0, 0xff, 0xff, 
	0x08, 0x00, 0xe0, 0xc3, 0x11, 0xc0, 0xff, 0xff, 0x04, 0x00, 0xc0, 0xc3, 0xc3, 0xc0, 0xff, 0xff, 
	0x04, 0x00, 0xc0, 0xc3, 0xc3, 0xe0, 0xff, 0xff, 0x04, 0x00, 0xe0, 0xc3, 0x53, 0xe0, 0xff, 0xff, 
	0x06, 0x00, 0xe0, 0xc3, 0x73, 0xe0, 0xff, 0xff, 0x06, 0x00, 0xe0, 0xc3, 0x73, 0xe0, 0xff, 0xff, 
	0x06, 0x00, 0xe0, 0xc3, 0x71, 0xe0, 0xff, 0xff, 0x06, 0x00, 0xe0, 0xe3, 0x31, 0xe0, 0xff, 0xff, 
	0x03, 0x00, 0xe0, 0xe3, 0x31, 0xe0, 0xff, 0xff, 0x03, 0x00, 0xe0, 0xe3, 0x03, 0xe0, 0xff, 0xff, 
	0x03, 0x00, 0xc0, 0xe3, 0x03, 0xe0, 0xff, 0xff, 0x03, 0x00, 0xc0, 0xf3, 0x07, 0xe0, 0xff, 0xff, 
	0x01, 0x00, 0x80, 0xf7, 0x07, 0xf0, 0xff, 0xff, 0x01, 0x00, 0x80, 0xff, 0x03, 0xf0, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0xff, 0x03, 0xf8, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0xf8, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0xff, 0x00, 0xfc, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xfc, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0x7f, 0x00, 0xfe, 0xff, 0xfb, 0x00, 0x00, 0x04, 0x3f, 0x00, 0xff, 0xff, 0xfb, 
	0x00, 0x00, 0x04, 0x0f, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x04, 0x1f, 0x80, 0xff, 0xff, 0xff, 
	0x00, 0x00, 0x0c, 0x3e, 0x80, 0xff, 0xff, 0xff, 0x00, 0x00, 0x0c, 0x76, 0xc0, 0xff, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0xfa, 0xe2, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xf8, 0xb8, 0xff, 0xff, 0xff, 
	0x00, 0x00, 0x00, 0xf8, 0xe0, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x7c, 0xe0, 0xff, 0xff, 0xff
};

    ssd1306_t ssd_bm;
    ssd1306_init_bm(&ssd_bm, 128,64, false, 0x3C, i2c1);
    ssd1306_config(&ssd_bm);

    ssd1306_draw_bitmap(&ssd_bm, bitmap_128x64);


    while(true) {
        sleep_ms(1000);
    }

    return 0;
}