#include "buttons.h"
#include <libpic30.h>

volatile uint8_t btn_flag = 0;

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {
    __delay32(200000); // debounce ~20ms

    if (PORTDbits.RD13 == 0) btn_flag = 1;  // poprzedni
    else if (PORTDbits.RD6 == 0) btn_flag = 2;  // nast?pny

    while (PORTDbits.RD6 == 0 || PORTDbits.RD13 == 0);
    IFS1bits.CNIF = 0;
}

void init_buttons() {
    TRISD |= (1 << 13) | (1 << 6);     // ustaw wej?cia
    CNPU2bits.CN19PUE = 1;             // pull-up RD13
    CNPU1bits.CN15PUE = 1;             // pull-up RD6
    CNEN2bits.CN19IE = 1;              // enable RD13
    CNEN1bits.CN15IE = 1;              // enable RD6
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

void update_mode(volatile uint8_t *mode) {
    if (btn_flag == 1) {
        *mode = (*mode == 0) ? 1 : *mode - 1;
        btn_flag = 0;
    } else if (btn_flag == 2) {
        *mode = (*mode + 1) % 2;
        btn_flag = 0;
    }
}

uint8_t button_pressed() {
    return btn_flag != 0;
}
