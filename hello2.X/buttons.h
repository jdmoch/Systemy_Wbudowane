#ifndef BUTTONS_H
#define BUTTONS_H

#include <xc.h>
#include <stdint.h>

void init_buttons();
void update_mode(volatile uint8_t *mode);
uint8_t button_pressed();

#endif