/*
 * File:   main.c
 * Author: utente
 *
 * Created on March 20, 2024, 4:39 PM
 */


#include <p33EP512MU810.h>

#include "xc.h"
#include "timer.h"

/*
// Ex.1: Interrupt function
void __attribute__((__interrupt__, no_auto_psv__)) _T2Interrupt(){
    
    TMR2 = 0;               // Reset timer
    IFS0bits.T2IF = 0;      // Reset timer2 flag 
    LATGbits.LATG9 = (!LATGbits.LATG9);     // Change status led2
}
*/

// Ex.2: Interrupt functions
void __attribute__((__interrupt__, no_auto_psv__)) _INT1Interrupt(){
    
    IFS1bits.INT1IF = 0;            // Reset button flag
    IEC0bits.T2IE = 1;              // Enable T2 interrupt
    IEC1bits.INT1IE = 0;            // Disable button
    tmr_setup_period(TIMER2, 30,1);
}

void __attribute__((__interrupt__, no_auto_psv__)) _T2Interrupt(){
    
    IFS0bits.T2IF = 0;      // Reset timer2 flag
    IEC0bits.T2IE = 0;      // Disable timer2
    T2CONbits.TON = 0;      // Stop the timer
    IFS1bits.INT1IF = 0;    // Reset button flag
    IEC1bits.INT1IE = 1;    // Enable button
    
    if (PORTEbits.RE8 == 1){                     
        LATGbits.LATG9 = (!LATGbits.LATG9);
    }
}


int main(void) {
    
    // all analog pins disabled
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;

    TRISAbits.TRISA0 = 0;   // Set LD1 as ouptut
    TRISGbits.TRISG9 = 0;   // Set LD2 as output
    TRISEbits.TRISE8 = 1;   // Set button T2 as input
    LATAbits.LATA0 = 0; // Value for switch on or off the LD1
    LATGbits.LATG9 = 0;     // Value of LD2
    
    RPINR0bits.INT1R = 0x58;    // Remapping of pin to interrupt
    INTCON2bits.GIE = 1;        // General interrupt enable
    IFS1bits.INT1IF = 0;        // Interrupt flag
    IEC1bits.INT1IE = 1;        // Interrupt enable
    
    tmr_setup_period(TIMER1, 200,1);      // Setup timer
    
    while(1){
        if(tmr_wait_period(TIMER1))
            LATAbits.LATA0 = (!LATAbits.LATA0);
    }

    return 0;
}