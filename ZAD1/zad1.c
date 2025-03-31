// PIC24FJ128GA010 Configuration Bit Settings
// For more on Configuration Bits
// consult your device data sheet
// CONFIG2
#pragma config POSCMOD = XT // XT Oscillator mode selected
#pragma config OSCIOFNC = ON // OSC2/CLKO/RC15 as port I/O (RC15)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor disabled
#pragma config FNOSC = PRI // Primary Oscillator (XT, HS, EC)
#pragma config IESO = ON // Int Ext Switch Over Mode enabled
// CONFIG1
#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128 // WDT Prescaler (1:128)
#pragma config WINDIS = ON // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF // Watchdog Timer disabled
#pragma config ICS = PGx2 // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF // Writes to program memory allowed
#pragma config GCP = OFF // Code protection is disabled
#pragma config JTAGEN = OFF // JTAG port is disabled
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include <libpic30.h>
#include "buttons.h"

unsigned char helper9(unsigned char seed);


int main(void) {
    AD1PCFG = 0xFFFF; // set to digital I/O (not analog)
    TRISA = 0x0000; // set all port bits to be output
    char button = 0b00000001;
    while(1){
        if(BUTTON_IsPressed (BUTTON_S3) == true){
            button--;
            if(button == 0b00000000) button = 0b00001001;
            __delay32(1000000);
        } else if(BUTTON_IsPressed (BUTTON_S4) == true){
            button++;
            if(button == 0b00001010) button = 0b00000001;
            __delay32(1000000);
        }
        switch(button){
            case 1:
                f1();
                break;
            case 2:
                f2();
                break;
            case 3:
                f3();
                break;
            case 4:
                f4();
                break;
            case 5:
                f5();
                break;
            case 6:
                f6();
                break;
            case 7:
                f7();
                break;
            case 8:
                f8();
                break;
            case 9:
                f9();
                break;
        }
    }
}

int f1(){
    unsigned char value = 0b00000000;
    LATA = value;
    while(1){
        __delay32(1000000);
        LATA = ++value;
        if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
    }
}


int f2(){
    unsigned char value = 0b11111111;
    LATA = value;
    while(1) {
        __delay32(1000000);
        LATA = --value;
        if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
    }
}

int f3(){
        unsigned char value = 0b00000000;
        unsigned char gray = 0b00000000;
        LATA = gray;
        while(1){
            __delay32(1000000);
            value++;
            gray = value ^ (value >> 1);
            LATA = gray;
            if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
        }
}

int f4(){
        unsigned char value = 0b11111111;
        unsigned char gray = 0b10000000;
        LATA = gray;
        while(1){
            __delay32(1000000);
            value--;
            gray = value ^ (value >> 1);
            LATA = gray;
            if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
        }
}

int f5(){
        unsigned char value = 0b00000000;
        unsigned char bcd = 0b00000000;
        LATA = bcd;
        while(1){
            __delay32(1000000);
            value++;
            bcd = (((value / 10) << 4) | (value % 10));
            LATA = bcd;
            if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
        }
}

int f6(){
        unsigned char value = 0b01100011;
        unsigned char bcd = 0b10011001;
        LATA = bcd;
        while(1){
            __delay32(1000000);
            value--;
            bcd = (((value / 10) << 4) | (value % 10));
            LATA = bcd;
            if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
        }
}

int f7(){
        unsigned char snake = 0b00000111;
        while(snake != 0b11100000){
            LATA = snake;
            snake <<= 1;
            __delay32(1000000);
            if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
        }
        while(snake != 0b00000011){
            LATA = snake;
            snake >>= 1;
            __delay32(1000000);
            if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
        }
}

int f8(){
    while(1){
        unsigned char value = 0b00000000;
        for(int i = 0; i < 8; i++){
            unsigned char temp = 0b00000001;
            for(int j = i + 1; j < 8; j++){
                LATA = value + temp;
                temp <<= 1;
                __delay32(1000000);
                if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
            }
            LATA = value + temp;
            value += temp;
            __delay32(1000000);
            if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
        }
    }
}

int f9(){
    unsigned char value = 0b01100100;
    while(1){
        value = helper9(value);
        LATA = value;
        __delay32(1000000);
        if(BUTTON_IsPressed (BUTTON_S3) || BUTTON_IsPressed (BUTTON_S4)) return 0;
    }
}

unsigned char helper9(unsigned char seed){
    return (((seed & (1 << 6)) ^ (seed & (1 << 5)) ^
        (seed & (1 << 4)) ^ (seed & (1 << 1)) ^ (seed & 1)) << 6) | (seed >> 1);
}