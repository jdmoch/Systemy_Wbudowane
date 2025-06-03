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

#define FCY 4000000UL
#define DEBOUNCE_DELAY 200000
#define buttonnext PORTDbits.RD6 
#define buttonprev PORTDbits.RD13

unsigned char tryb = 1;
unsigned long delay_value = 500000;

uint16_t potencjometr() { 
    AD1CON1bits.SAMP = 1;   
    __delay32(100);         
    AD1CON1bits.SAMP = 0;   
    while (!AD1CON1bits.DONE); 
    return ADC1BUF0;       
}

void predkosc() {
    uint16_t pot_value = potencjometr();
    
    if (pot_value < 205) {          
        delay_value = 1000000;      
    } else if (pot_value < 410) {   
        delay_value = 750000;      
    } else if (pot_value < 615) {   
        delay_value = 500000;       
    } else if (pot_value < 820) {   
        delay_value = 250000;       
    } else {                       
        delay_value = 100000;       
    }
}

void funk3() {
    unsigned char licznik_normalny = 0; 
    unsigned char wynik_gray; 
    
    while (1) { 
        predkosc();
        
        wynik_gray = licznik_normalny ^ (licznik_normalny >> 1);
        
        LATA = wynik_gray; 
        __delay32(delay_value); 
        
        if (!buttonnext || !buttonprev) {
            return; 
        }
        licznik_normalny = licznik_normalny + 1;
    }
}

void funk7() {
    unsigned char wezyk; 
    unsigned char kierunek_w_prawo = 1; 
    
    wezyk = 0b00000111; 
    
    while (1) { 
        predkosc();
        
        LATA = wezyk; 
        __delay32(delay_value); 
        
        if (!buttonnext || !buttonprev) {
            return; 
        }
        
        if (kierunek_w_prawo == 1) {
            wezyk = wezyk << 1;
            
            if (wezyk == 0b11100000) {
                kierunek_w_prawo = 0;
            }
        } else { 
            wezyk = wezyk >> 1;
            
            if (wezyk == 0b00000111) {
                kierunek_w_prawo = 1;
            }
        }
    }
}

int main(void) {

    TRISA = 0x0000;  
    TRISD = 0xFFFF;   
    
    AD1PCFG = 0xFFDF;        
    AD1CON1 = 0x00E0;         
    AD1CON2 = 0;             
    AD1CON3 = 0x1F3F;        
    AD1CHS = 5;              
    AD1CON1bits.ADON = 1;   
    
    __delay32(1000);     

    while (1) {
        switch (tryb) {
            case 1:
                funk3();  
                break;
            case 2:
                funk7(); 
                break;
        }

        if (!buttonnext) {
            __delay32(DEBOUNCE_DELAY);    
            if (!buttonnext) {                
                tryb++;
                if (tryb > 2)
                    tryb = 1;
                while (!buttonnext);       
                __delay32(DEBOUNCE_DELAY);     
            }
        }

        if (!buttonprev) {
            __delay32(DEBOUNCE_DELAY);        
            if (!buttonprev) {
                if (tryb == 1)
                    tryb = 2;
                else
                    tryb--;
                while (!buttonprev);      
                __delay32(DEBOUNCE_DELAY);    
            }
        }
    }
}