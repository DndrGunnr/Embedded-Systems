/*
 * File:   main.c
 * Author: Family
 *
 * Created on April 29, 2024, 7:58 AM
 */


#include "xc.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

// ---------------------------------------- TO ADD ----------------------------------- //
/* 1) Controllo dell'interrupt di OERR per evitare overflow di RX reg
 * 2) Valutare bene tutti i possibili controlli degli interrupt 
 * 3) input per aggiunta o rimozioni del PARITY bit
 * 4) Funzione di invio di n char dalla testa in poi
 * 5) Auto selezione del BAUDE rate
 * 6) --- altro ???---
 */
// ---------------------------------------- TO ADD ----------------------------------- //


uint16_t new_char;

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0; // reset the flag of the RX reg
    
    new_char = 1;
    uart_buff_add();
}

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0; // reset the flag of the TX reg
}

int main(void) {
    // disable analog pin
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // LED SET UP
    TRISGbits.TRISG9 = 0; // output
    TRISAbits.TRISA0 = 0; // output
    LATGbits.LATG9 = 0; // start in OFF state
    LATAbits.LATA0 = 0; // start in OFF state
    
    uart_setup(1);
    new_char = 0;
    
    while(1){
        if(new_char == 1){
            uart_send_head();
            uart_buff_rmv();
            new_char = 0;
        }
    }
    return 0;
}
