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

int16_t toSend_char = 0; // varaibile di conteggio per sapere quanti char mandare


void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // flag to zero
    LATGbits.LATG9 = (!LATGbits.LATG9);// led flik to se interr action
    
    uart_send_head();
    uart_buff_rmv();
    toSend_char = toSend_char - 1;
    
    IEC0bits.U1TXIE = 0; // dopo aver inviato un elemento spengo interrupt
}

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0; // flag to zero
    
    // carattere ricevuto messo nel buffer
    uart_buff_add();
    // incremento valore di toSend_char;
    toSend_char = toSend_char + 1;
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
        if(toSend_char == 0){
             // accendo led
            // se il buffer è vuoto disattivo l'interrupt di invio
        }else{
            IEC0bits.U1TXIE = 1;
            // se il buffer ha degli elementi attivo l'interrupt di invio
        }
    }
    
    return 0;
}

// COMMIT 1 COMPORTAMENTO:
    /*inizia con comportamento corretto, dopo aver inviato qualche carattere comincia a 
     stampare 2 caratteri per volta (caratteri extra sembrano essere in un ciclo con 
     * caratteri precedenti) err__if buff_empty settava ENABLE TX a zero*/

// COMMIT 2 COMPORTAMENTO:
    


