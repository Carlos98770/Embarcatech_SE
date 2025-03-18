#ifndef JOYSTICK_LED_H
#define JOYSTICK_LED_H

void setup_joystick();
void setup_pwm_led(uint led, uint *slice, uint16_t level);
void setup();
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value);
void progama_1();

#endif // JOYSTICK_LED_H
