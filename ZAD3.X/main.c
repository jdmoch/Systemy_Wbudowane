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
#define button_wylacz PORTDbits.RD6 
#define NASTAWA_ALARM 512      
#define OPOZNIENIE_MRUGANIA 500000

uint16_t potencjometr() { 
    AD1CON1bits.SAMP = 1;   
    __delay32(100);         
    AD1CON1bits.SAMP = 0;   
    while (!AD1CON1bits.DONE); 
    return ADC1BUF0;       
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

    unsigned char stan_alarmu = 0;     
    unsigned long licznik_5s = 0;       
    unsigned char dioda_stan = 0;      
    uint16_t wartosc_pot;
    
    while (1) {
        wartosc_pot = potencjometr();
        
        if (stan_alarmu == 0 && wartosc_pot > NASTAWA_ALARM) {
            stan_alarmu = 1;
            licznik_5s = 0;
            dioda_stan = 0;
        }
        
        if (stan_alarmu > 0 && wartosc_pot <= NASTAWA_ALARM) {
            stan_alarmu = 0;
            LATA = 0x0000;  
            licznik_5s = 0;
        }
        
        if (!button_wylacz && stan_alarmu > 0) {
            __delay32(DEBOUNCE_DELAY);
            if (!button_wylacz) {
                stan_alarmu = 0;
                LATA = 0x0000; 
                licznik_5s = 0;
                while (!button_wylacz); 
                __delay32(DEBOUNCE_DELAY);
            }
        }
        
        switch (stan_alarmu) {
            case 0:  
                LATA = 0x0000;
                break;
                
            case 1:  
                if (dioda_stan == 0) {
                    LATA = 0x0001;  
                    dioda_stan = 1;
                } else {
                    LATA = 0x0000;  
                    dioda_stan = 0;
                }
                
                __delay32(OPOZNIENIE_MRUGANIA);
                licznik_5s++;
                
                if (licznik_5s >= 40) {
                    stan_alarmu = 2;
                }
                break;
                
            case 2:  
                LATA = 0x00FF;  
                __delay32(100000);
                break;
        }
    }
    
    return 0;
}