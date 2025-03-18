#ifndef INTERFACE_H
#define INTERFACE_H


void init_I2c();
void limpar_oled();
void atualizar_status(float eficacia, float angulo_x, float angulo_y);
void imprimir_inicializacao();
void imprimir_status();


#endif // INTERFACE_H