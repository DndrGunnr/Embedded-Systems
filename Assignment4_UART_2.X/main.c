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
    // Setup UART
    //pin mapping
    //mapping of remappable pin RP64 on pin RD0 to work as UART1TX
    RPOR0bits.RP64R = 1; 
    //mapping of remappable pin RPI75 on pin RD11 to work as UA1RX
    RPINR18bits.U1RXR = 0x4b; // 0x4b is 75 in hexadecimal
    //UART1 init
    //BaudRate selection
    //U1BRG = (FCY/(16*baud))-1; //(72MHz/(16*9600))-1
    U1BRG = 468;
    U1MODEbits.UARTEN = 1; //global UART enable
    U1STAbits.UTXEN = 1; //UART1 enable
    IEC0bits.U1RXIE = 1; //UART1 interrupt enable
    
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
    //uart_setup(9600);
    
    while(1) {
        algorithm();
        // code to handle the assignment
        ret = tmr_wait_period(TIMER1);
    }
    return 0;
}