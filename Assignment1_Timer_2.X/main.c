/*
 * File:   main.c
 * Author: enric
 *
 * Created on April 10, 2024, 9:46 AM
 */


#include "xc.h"
#include "timer.h"


// first exe main
int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;
    // disable analog pins
    
    // led initialization
    TRISAbits.TRISA0 = 0; // set to output led 1
    
    LATAbits.LATA0 = 0;
    while(1){
        // turn led ON and wait
        LATAbits.LATA0 = 1;
            
        tmr_wait_ms(TIMER2, 20); 

 
 
        // turn led ON and wait
        LATAbits.LATA0 = 0;
            
        tmr_wait_ms(TIMER2, 200); 

    }
    return 0;
}
