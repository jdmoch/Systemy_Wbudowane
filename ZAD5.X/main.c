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
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "adc.h"

#define FCY 4000000UL
#define DEBOUNCE_DELAY 200000
#define PRZYCISK_GRACZ1 PORTDbits.RD6
#define PRZYCISK_GRACZ2 PORTDbits.RD13

unsigned int czas_g1 = 0;
unsigned int czas_g2 = 0;
unsigned char aktywny_gracz = 0;
unsigned int wybrany_czas = 300; 

volatile bool przycisk_g1_flag = false;
volatile bool przycisk_g2_flag = false;
volatile bool przycisk_flag = false;

void __attribute__((interrupt, auto_psv)) _CNInterrupt(void) {
    if (!PRZYCISK_GRACZ1) {  
        przycisk_g1_flag = true;
    }
    if (!PRZYCISK_GRACZ2) { 
        przycisk_g2_flag = true;
    }
    przycisk_flag = true;
    IFS1bits.CNIF = 0;
}

void opoznienie_ms(unsigned int ms) {
    while (ms--) {
        __delay32(FCY / 1000);
    }
}

void pokaz_czas() {
    char bufor[33];
    char wskaznik1 = (aktywny_gracz == 1) ? '>' : ' ';
    char wskaznik2 = (aktywny_gracz == 2) ? '>' : ' ';
    
    sprintf(bufor, "%cG1: %02u:%02u\n%cG2: %02u:%02u",
            wskaznik1, czas_g1 / 60, czas_g1 % 60,
            wskaznik2, czas_g2 / 60, czas_g2 % 60);
    LCD_ClearScreen();
    LCD_PutString(bufor, strlen(bufor));
}

void komunikat(char* msg) {
    LCD_ClearScreen();
    LCD_PutString(msg, strlen(msg));
}

void pokaz_ustawienia_czasu() {
    char bufor[33];
    sprintf(bufor, "Nastaw czas:\n%u:%02u min", wybrany_czas / 60, wybrany_czas % 60);
    LCD_ClearScreen();
    LCD_PutString(bufor, strlen(bufor));
}

unsigned int odczytaj_czas_z_potencjometru() {
    uint16_t pot = ADC_Read10bit(ADC_CHANNEL_5);
    if (pot < 341) {
        return 60;  
    } else if (pot < 682) {
        return 180; 
    } else {
        return 300; 
    }
}

void konfiguruj_przerwania() {

    CNEN1bits.CN15IE = 1;  
    CNEN2bits.CN19IE = 1;  
    
    IPC4bits.CNIP = 2; 
    IFS1bits.CNIF = 0;  
    IEC1bits.CNIE = 1; 
}

void petla_ustawiania_czasu() {
    unsigned int poprzedni_czas = 0;
    
    komunikat("Ustaw czas");
    opoznienie_ms(1500);
    
    while (1) {
        wybrany_czas = odczytaj_czas_z_potencjometru();
        
        if (wybrany_czas != poprzedni_czas) {
            pokaz_ustawienia_czasu();
            poprzedni_czas = wybrany_czas;
        }
        
        if (przycisk_flag) {
            __delay32(DEBOUNCE_DELAY);
            
            if (przycisk_g1_flag && !PRZYCISK_GRACZ1) {
                czas_g1 = czas_g2 = wybrany_czas;
                przycisk_g1_flag = false;
                przycisk_flag = false;
                
                while (!PRZYCISK_GRACZ1) {
                    __delay32(10000);
                }
                __delay32(DEBOUNCE_DELAY);
                return;
            }
            
            if (przycisk_g2_flag && !PRZYCISK_GRACZ2) {
                czas_g1 = czas_g2 = wybrany_czas;
                przycisk_g2_flag = false;
                przycisk_flag = false;
                
                while (!PRZYCISK_GRACZ2) {
                    __delay32(10000);
                }
                __delay32(DEBOUNCE_DELAY);
                return;
            }
            
            przycisk_g1_flag = false;
            przycisk_g2_flag = false;
            przycisk_flag = false;
        }
        
        opoznienie_ms(1);
    }
}

void inicjalizacja() {
    TRISDbits.TRISD6 = 1;  
    TRISDbits.TRISD13 = 1;  
    TRISA = 0xFFFF;    
    
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_5);
    LCD_Initialize();
}

void petla_zegaru() {
    unsigned int licznik_sekund = 0;
    
    aktywny_gracz = 0;
    pokaz_czas();
    
    komunikat("Nacisnij aby\nzaczac");
    opoznienie_ms(2000);
    pokaz_czas();
    
    while (1) {
        if (przycisk_flag) {
            __delay32(DEBOUNCE_DELAY);
            
            if (przycisk_g1_flag && !PRZYCISK_GRACZ1) {
                aktywny_gracz = 2; 
                pokaz_czas();
                przycisk_g1_flag = false;
                
                while (!PRZYCISK_GRACZ1) {
                    __delay32(10000);
                }
                __delay32(DEBOUNCE_DELAY);
            }
            
            if (przycisk_g2_flag && !PRZYCISK_GRACZ2) {
                aktywny_gracz = 1;
                pokaz_czas();
                przycisk_g2_flag = false;
                
                while (!PRZYCISK_GRACZ2) {
                    __delay32(10000);
                }
                __delay32(DEBOUNCE_DELAY);
            }
            
            przycisk_flag = false;
        }
        
        licznik_sekund++;
        if (licznik_sekund >= 100) {
            licznik_sekund = 0;
            
            if (aktywny_gracz == 1 && czas_g1 > 0) {
                czas_g1--;
                pokaz_czas();
            } else if (aktywny_gracz == 2 && czas_g2 > 0) {
                czas_g2--;
                pokaz_czas();
            }
            
            if (czas_g1 == 0 && aktywny_gracz == 1) {
                komunikat("Gracz 1 \nPRZEGRAL");
                break;
            }
            if (czas_g2 == 0 && aktywny_gracz == 2) {
                komunikat("Gracz 2 \nPRZEGRAL!");
                break;
            }
        }
        
        opoznienie_ms(10); 
    }
}

int main(void) {
    inicjalizacja();
    konfiguruj_przerwania();
    
    petla_ustawiania_czasu();
    
    komunikat("Czas ustawiony");
    opoznienie_ms(2000);
    
    petla_zegaru();
    
    while (1) {
        opoznienie_ms(1000);
    }
    
    return 0;
}