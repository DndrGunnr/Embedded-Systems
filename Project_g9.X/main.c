/*
 * File:   main.c
 * Author: enric
 *
 * Created on June 11, 2024, 2:49 PM
 */


#include "xc.h"
#include "timer.h"
#include "uart.h"
#include "scheduler.h"

int is_waiting=1; //wait status
int led_blink_counter=0; //in order to use the same timer for the main loop and the led blinking

void __attribute__((__interrupt__, __no_auto_psv__)) _INT1Interrupt(){
    
    IFS1bits.INT1IF = 0;            // Reset button flag
    IEC1bits.INT1IE = 0;            // Disable button
    IEC0bits.T1IE = 1;              // Enable T2 interrupt
    tmr_setup_period(TIMER1, 30);
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T1Interrupt(){
    
    IFS0bits.T1IF = 0;     
    T1CONbits.TON = 0;      // Stop the timer
    IFS1bits.INT1IF = 0;    // Reset button flag
    IEC1bits.INT1IE = 1;    // Enable button
    
    if (PORTEbits.RE8==1) //value is 1 when button is not pressed
        is_waiting=(!is_waiting);
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1TXInterrupt(){
    IFS0bits.U1TXIF = 0;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(){
    IFS0bits.U1RXIF = 0;
}

/*
 * void __attribute__((__interrupt__, __no_auto_psv__)) _T3Interrupt(){
    TMR3=0;
    IFS0bits.T3IF=0;
    led_blink_counter++;
    if(led_blink_counter==1000){
        led_blink_counter=0;
        LATAbits.LATA0=(!LATAbits.LATA0);
        if(is_waiting){
            LATBbits.LATB8=(!LATBbits.LATB8);
            LATFbits.LATF1=(!LATFbits.LATF1);
        }   
    }
}

 */

void setup(){
    ANSELA=ANSELB=ANSELC=ANSELD=ANSELE=ANSELG=0x0000;
    TRISEbits.TRISE8=1; //button for status toggle set as input
    TRISAbits.TRISA0=0; //ledA0 set to output
    RPINR0bits.INT1R=0x58; //interrupt1 remapped to interrupt
    INTCON2bits.GIE=1;
    IFS1bits.INT1IF = 0;
    IEC1bits.INT1IE=1;
    //turning lights pins
    TRISFbits.TRISF1=0;
    TRISBbits.TRISB8=0;
    tmr_setup_period(TIMER3, 1);
    IEC0bits.T3IE=0;
    //uart setup
    //int TX_interrupt_on, int TX_interrupt_type, int RX_interrupt_on, int RX_interrupt_type
    uart_setup(1,0,1,0);
    
    // scheduler initialization
    init_scheduler();
}



int main(void) {
    setup();
    
    
    while(1){
        scheduler(is_waiting);
        tmr_wait_period(TIMER3);
    }
    return 0;
}
