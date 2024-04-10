/*
 * File:   main.c
 * Author: enric
 *
 * Created on April 10, 2024, 9:51 AM
 */


int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    // disable analog pins
    
    // led initialization
    TRISAbits.TRISA0 = 0; // set to output led 1
    TRISGbits.TRISG9 = 0; // set to output led 2
    
    LATAbits.LATA0 = 0;
    LATGbits.LATG9 = 0;
    
    while(1){
        LATAbits.LATA0 = 1;
        tmr_wait_ms(TIMER1, 50);
        LATAbits.LATA0 = 0;
        
        LATGbits.LATG9 = 1;
        tmr_wait_ms(TIMER1, 200);
        LATGbits.LATG9 = 0;
        
        LATAbits.LATA0 = 1;
        tmr_wait_ms(TIMER1, 2000);
        LATAbits.LATA0 = 0;
        
        tmr_wait_ms(TIMER1, 2000);
    }
    
    
    return 0;
} 
