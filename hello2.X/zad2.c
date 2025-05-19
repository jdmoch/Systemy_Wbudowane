// CONFIGURATION BITS
#pragma config POSCMOD = NONE
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = FRC
#pragma config IESO = OFF
#pragma config WDTPS = PS32768
#pragma config FWPSA = PR128
#pragma config WINDIS = ON
#pragma config FWDTEN = OFF
#pragma config ICS = PGx2
#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF

#include <xc.h>
#include <libpic30.h>
#include <stdlib.h>
#include "buttons.h"

#define FCY 8000000UL
#define SYS_FREQ 8000000UL

volatile uint8_t mode = 0;
volatile uint16_t pot_value = 0;

void delay() {
    uint32_t loops;

    if (pot_value > 820)      loops = 1000;
    else if (pot_value > 615) loops = 8000;
    else if (pot_value > 410) loops = 3000;
    else if (pot_value > 205) loops = 10000;
    else                      loops = 5000;

    for (uint32_t i = 0; i < loops; i++) {
        __delay32(50);
    }
}

void adc_init() {
    AD1PCFG = 0xFFDF;   // AN5 as analog input
    AD1CHS = 5;         // Select AN5
    AD1CON1 = 0x00E0;   // Auto-convert
    AD1CON2 = 0;
    AD1CON3 = 0x1F3F;
    AD1CON1bits.ADON = 1;
}

uint16_t read_pot() {
    AD1CON1bits.SAMP = 1;
    __delay32(100);
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE);
    return ADC1BUF0;
}

void snake_pattern() {
    static uint8_t segment = 0b00000111;
    static int8_t direction = 1;

    pot_value = read_pot();
    LATA = segment;
    delay();

    if (direction > 0) {
        segment <<= 1;
        if (segment > 0b01111000) {
            segment = 0b11100000;
            direction = -1;
        }
    } else {
        segment >>= 1;
        if (segment < 0b00000111) {
            segment = 0b00001110;
            direction = 1;
        }
    }
}

void countdown_pattern() {
    static uint8_t value = 255;

    pot_value = read_pot();
    LATA = value;
    delay();

    if (value == 0) value = 255;
    else value--;
}

int main() {
    TRISA = 0x0000;
    LATA = 0;
    adc_init();
    init_buttons(); 

    while (1) {
        update_mode(&mode);  

        switch (mode) {
            case 0: snake_pattern(); break;
            case 1: countdown_pattern(); break;
            default: mode = 0; break;
        }
    }
}
