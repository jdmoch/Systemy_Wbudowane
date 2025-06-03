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
#define delay 500000
#define DEBOUNCE_DELAY 200000
#define buttonnext PORTDbits.RD6 
#define buttonprev PORTDbits.RD13

unsigned char tryb = 1;
volatile bool button_pressed = false;  // Flaga zmiany trybu

// Funkcja sprawdzaj?ca przyciski (bez return!)
bool check_buttons() {
    static bool next_pressed = false;
    static bool prev_pressed = false;
    
    // Sprawd? przycisk NEXT
    if (!buttonnext && !next_pressed) {
        __delay32(DEBOUNCE_DELAY);
        if (!buttonnext) {  // Potwierd? po debounce
            next_pressed = true;
            tryb++;
            if (tryb > 9) tryb = 1;
            button_pressed = true;
            return true;  // Sygnalizuj zmian? trybu
        }
    } else if (buttonnext) {
        next_pressed = false;  // Reset flagi gdy przycisk zwolniony
    }
    
    // Sprawd? przycisk PREV
    if (!buttonprev && !prev_pressed) {
        __delay32(DEBOUNCE_DELAY);
        if (!buttonprev) {  // Potwierd? po debounce
            prev_pressed = true;
            if (tryb == 1) 
                tryb = 9;
            else 
                tryb--;
            button_pressed = true;
            return true;  // Sygnalizuj zmian? trybu
        }
    } else if (buttonprev) {
        prev_pressed = false;  // Reset flagi gdy przycisk zwolniony
    }
    
    return false;  // Brak zmiany trybu
}

void funk1() {
    unsigned char licznik = 0; 
    
    while (1) {
        LATA = licznik; 
        __delay32(delay); 
       
        if (check_buttons()) {
            return;  // Wyj?cie tylko przy zmianie trybu
        }
        licznik = licznik + 1;
    }
}

void funk2() {
    unsigned char licznik = 255; 
    
    while (1) {
        LATA = licznik; 
        __delay32(delay);
        
        if (check_buttons()) {
            return;
        }
        licznik = licznik - 1;
    }
}

void funk3() {
    unsigned char licznik_normalny = 0; 
    unsigned char wynik_gray; 
    
    while (1) { 
        wynik_gray = licznik_normalny ^ (licznik_normalny >> 1);
        
        LATA = wynik_gray; 
        __delay32(delay); 
        
        if (check_buttons()) {
            return; 
        }
        licznik_normalny = licznik_normalny + 1;
    }
}

void funk4() {
    unsigned char i; 
    unsigned char kod_gray; 
    i = 255;
    while (1) { 
        kod_gray = i ^ (i >> 1);  
        LATA = kod_gray; 
        __delay32(delay);
        
        if (check_buttons()) {
            return;
        }
        i = i - 1;  
    }
}

void funk5() {
    unsigned char dziesiatki; 
    unsigned char jednosci;   
    unsigned char wynik_bcd;
    unsigned char licznik_dziesiatkowy = 0; 
    
    while (1) {
        dziesiatki = licznik_dziesiatkowy / 10;
        jednosci = licznik_dziesiatkowy % 10;
        wynik_bcd = (dziesiatki << 4) | jednosci;
        LATA = wynik_bcd; 
        __delay32(delay); 
        
        if (check_buttons()) {
            return;
        }
        licznik_dziesiatkowy++;
        
        if (licznik_dziesiatkowy > 99) {
            licznik_dziesiatkowy = 0;
        }
    }
}

void funk6() {
    unsigned char dziesiatki; 
    unsigned char jednosci;    
    unsigned char bcd;        
    unsigned char licznik = 99; 
    
    while (1) {
        dziesiatki = licznik / 10; 
        jednosci = licznik % 10;
        bcd = (dziesiatki * 16) + jednosci; 
        LATA = bcd; 
        __delay32(delay);
        
        if (check_buttons()) {
            return; 
        }
        licznik--;
        if (licznik == 255) { 
            licznik = 99; 
        }
    }
}

void funk7() {
    unsigned char wezyk; 
    unsigned char kierunek_w_prawo = 1; 
    
    wezyk = 0b00000111; 
    
    while (1) { 
        LATA = wezyk; 
        __delay32(delay); 
        
        if (check_buttons()) {
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

void funk8() {
    unsigned char stan_diod = 0;
    unsigned char pozycja; 
    unsigned char maska;
    
    for (int numer_diody = 0; numer_diody < 8; numer_diody++) {    
        maska = 1; 
        
        for (int i = numer_diody + 1; i < 8; i++) {
            LATA = stan_diod | maska;
            maska = maska << 1; 
            __delay32(delay); 
            if (check_buttons()) {
                return;
            }
        }

        stan_diod = stan_diod | maska;
        LATA = stan_diod;
        __delay32(delay);
        if (check_buttons()) {
            return;
        }
    }
}

void funk9() {
    unsigned char lfsr = 0b000001;      
    while (1) {
        unsigned char wartosc = ((lfsr >> 5) & 1) ^  
                                ((lfsr >> 4) & 1) ^    
                                ((lfsr >> 3) & 1) ^   
                                ((lfsr >> 1) & 1) ^   
                                ((lfsr >> 0) & 1);    
        
        lfsr = (lfsr >> 1) | (wartosc << 5);
        
        LATA = lfsr & 0b00111111;
        
        __delay32(delay);
        
        if (check_buttons())
            return;
    }
}

int main(void) {
    AD1PCFG = 0xFFFF;
    TRISA = 0x0000;
    TRISD = 0xFFFF;

    while (1) {
        button_pressed = false;  
        
        switch (tryb) {
            case 1:
                funk1(); 
                break;
            case 2:
                funk2(); 
                break;
            case 3:
                funk3();
                break;
            case 4:
                funk4();
                break;
            case 5:
                funk5();
                break;
            case 6:
                funk6();
                break;
            case 7:
                funk7();
                break;
            case 8:
                funk8();
                break;
            case 9:
                funk9();
                break;
        }
        
        __delay32(DEBOUNCE_DELAY);
    }
}