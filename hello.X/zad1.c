// PIC24FJ128GA010 Configuration Bit Settings
// CONFIG2
#pragma config POSCMOD = XT 
#pragma config OSCIOFNC = ON 
#pragma config FCKSM = CSDCMD 
#pragma config FNOSC = PRI 
#pragma config IESO = ON 
// CONFIG1
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
#include "buttons.h"
#include <stdbool.h>

int f1(void);
int f2(void);
int f3(void);
int f4(void);
int f5(void);
int f6(void);
int f7(void);
int f8(void);
int f9(void);
unsigned char helper9(unsigned char seed);

// Debouncing function
bool isButtonPressedDebounced(BUTTON button) {
    if (BUTTON_IsPressed(button)) {
        __delay32(300000); // ok. 10 ms opó?nienia (dla 30 MIPS)
        if (BUTTON_IsPressed(button)) {
            while (BUTTON_IsPressed(button)); // Czekaj a? przycisk zostanie puszczony
            __delay32(300000); // kolejne 10 ms
            return true;
        }
    }
    return false;
}

int main(void) {
    AD1PCFG = 0xFFFF; 
    TRISA = 0x0000; 
    char button = 0b00000001;

    while (1) {
        if (isButtonPressedDebounced(BUTTON_S3)) {
            button--;
            if (button == 0b00000000) button = 0b00001001;
        } 
        else if (isButtonPressedDebounced(BUTTON_S4)) {
            button++;
            if (button == 0b00001010) button = 0b00000001;
        }

        switch(button) {
            case 1: f1(); break;
            case 2: f2(); break;
            case 3: f3(); break;
            case 4: f4(); break;
            case 5: f5(); break;
            case 6: f6(); break;
            case 7: f7(); break;
            case 8: f8(); break;
            case 9: f9(); break;
        }
    }
}

int f1() {
    unsigned char value = 0b00000000;
    LATA = value;
    while(1){
        __delay32(1000000);
        LATA = ++value;
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
    }
}

int f2(){
    unsigned char value = 0b11111111;
    LATA = value;
    while(1) {
        __delay32(1000000);
        LATA = --value;
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
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
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
    }
}

int f4(){
    unsigned char value = 0b11111111;
    unsigned char gray = 0b11111111 ^ (0b11111111 >> 1);
    LATA = gray;
    while(1){
        __delay32(1000000);
        value--;
        gray = value ^ (value >> 1);
        LATA = gray;
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
    }
}

int f5(){
    unsigned char value = 0;
    unsigned char bcd;
    while(1){
        __delay32(1000000);
        value++;
        if(value > 99) value = 0;
        bcd = ((value / 10) << 4) | (value % 10);
        LATA = bcd;
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
    }
}

int f6(){
    unsigned char value = 99;
    unsigned char bcd = 0x99;
    while(1){
        __delay32(1000000);
        if(value == 0) value = 99;
        else value--;
        bcd = ((value / 10) << 4) | (value % 10);
        LATA = bcd;
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
    }
}

int f7(){
    unsigned char snake = 0b00000111;
    while(snake != 0b11100000){
        LATA = snake;
        snake <<= 1;
        __delay32(1000000);
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
    }
    LATA = snake;
    __delay32(1000000);
    while(snake != 0b00000111){
        snake >>= 1;
        LATA = snake;
        __delay32(1000000);
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
    }
    return f7();
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
        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4)) return 0;
    }
}

unsigned char helper9(unsigned char seed){
    unsigned char feedback = ((seed >> 6) & 1) ^ ((seed >> 5) & 1) ^ 
                             ((seed >> 4) & 1) ^ ((seed >> 1) & 1) ^ (seed & 1);
    return (feedback << 7) | (seed >> 1);
}
