#pragma config POSCMOD = XT         
#pragma config OSCIOFNC = ON        
#pragma config FCKSM = CSDCMD       
#pragma config FNOSC = PRI          
#pragma config IESO = ON

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
#include <stdbool.h>
#include <string.h>
#include "lcd.h"

#define FCY 4000000UL

void czekaj(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++) {
        for(j = 0; j < 1000; j++) {
            Nop();
        }
    }
}

void przewijanie(const char* tekst_gora, const char* tekst_dol, unsigned int szybkosc, unsigned int cykle) {
    char bufor_dol[17];
    int rozmiar_dol = strlen(tekst_dol);
    int pozycja;
    unsigned int i;
    
    char spacja[200];
    strcpy(spacja, "                "); 
    strcat(spacja, tekst_dol);
    strcat(spacja, "                "); 
    int pelny_rozmiar = strlen(spacja);
    
    for(i = 0; i < cykle; i++) {
        for (pozycja = 0; pozycja <= pelny_rozmiar - 16; pozycja++) {
            LCD_ClearScreen();
            
            LCD_PutString((char*)tekst_gora, strlen(tekst_gora));
            
            LCD_PutChar('\n');
            
            int j;
            for (j = 0; j < 16; j++) {
                if (pozycja + j < pelny_rozmiar) {
                    bufor_dol[j] = spacja[pozycja + j];
                } else {
                    bufor_dol[j] = ' ';
                }
            }
            bufor_dol[16] = '\0';
            
            LCD_PutString(bufor_dol, 16);
            
            czekaj(szybkosc);
        }
    }
}

void wyswietlanie1(void) {
    przewijanie("RASHED KEBAB!", "KEBAB TYLKO 15 ZLOTYCH! SWIEZE MIESO!", 200, 2);
}

void wyswietlanie2(void) {
    przewijanie(">> PROMOCJA <<", "KEBAB+COLA 20ZL - ul. Kowalskiego obok Biedronki", 180, 2);
}

void wyswietlanie3(void) {
    przewijanie("CZYNNE 10-3", "ZAMOW: 123-456-789 - DOSTAWA GRATIS!", 220, 2);
}

int main(void) {
    unsigned int i;
    for(i = 0; i < 10000; i++) {
        Nop();
    }
    
    LCD_Initialize();
    LCD_CursorEnable(false);       
    
    while (1) {
        wyswietlanie1();
        wyswietlanie2();
        wyswietlanie3();
        LCD_ClearScreen();
        czekaj(1000);
    }
    
    return 0;
}