#ifndef BUZZER_H
#define BUZZER_H

void pwm_init_buzzer();
void play_tone(uint pin, uint frequency, uint duration_ms);
int WaitWithRead_buzzer(int timeMS);
void progama_2();

#endif // BUZZER_H
