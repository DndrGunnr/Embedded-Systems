/*
 * File:   main.c
 * Author: Family
 *
 * Created on May 9, 2024, 10:15 PM
 */


#include "xc.h"
#include "timer.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

int16_t toSend_char; // varaibile di conteggio per sapere quanti char mandare


void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // flag to zero
    
    uart_send_head();
    uart_buff_rmv();
}

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0; // flag to zero
    
    // carattere ricevuto messo nel buffer
    uart_buff_add();
}



int main(void) {
    // analog pin set up 
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    // uart set up
    uart_setup(2); 
    
    // debug led set up
    TRISGbits.TRISG9 = 0;
    TRISAbits.TRISA0 = 0;
    
    while(1){
        if(uart_is_buff_empty()){
            LATGbits.LATG9 = 1; // accendo led
            // se il buffer è vuoto disattivo l'interrupt di invio
        }else{
            IEC0bits.U1TXIE = 1;
            // se il buffer ha degli elementi attivo l'interrupt di invio
        }
    }
    
    return 0;
}
