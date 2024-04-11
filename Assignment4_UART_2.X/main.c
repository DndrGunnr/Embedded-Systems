/*
 * File:   main.c
 * Author: utente
 *
 * Created on April 10, 2024, 10:03 AM
 */


#include "xc.h"
#include "timer.h"
#include "UART.h"

void __attribute__((__interrupt__, no_auto_psv__)) _T3Interrupt(){
    
    IFS0bits.T3IF = 0;      // Reset timer2 flag 
    TMR3=0;
    LATAbits.LATA0 = (!LATAbits.LATA0);
}

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(){
    char carattere;
    IFS0bits.U1RXIF=0;
    carattere=U1RXREG;
    U1TXREG=carattere;
    //LATAbits.LATA0 = (!LATAbits.LATA0);
    //Flag raised each time a character is received 
    U1STAbits.URXISEL=0;
}

void algorithm() {
    tmr_wait_ms(TIMER2, 7);
}

int main() {
    int ret;
    //LED2 as output
    TRISAbits.TRISA0 = 0;
    //LED2 init
    LATAbits.LATA0=0;
    tmr_setup_period(TIMER1, 10);
    //timer for LED2 blinking
    tmr_setup_period(TIMER3, 100);
    // activate enable for interrupts
    IEC0bits.T3IE = 1;
    // Setup UART
    uart_setup(9600);
    
    while(1) {
        algorithm();
        // code to handle the assignment
        ret = tmr_wait_period(TIMER1);
    }
    return 0;
}