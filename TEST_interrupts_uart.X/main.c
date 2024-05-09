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
#include "stdlib.h"
#include "time.h"

#define str_len 30

int16_t toSend_char = 0; // varaibile di conteggio per sapere quanti char mandare

char toSend[str_len]; // string to generate and send to UART
int16_t head = 0; // stirng index
int16_t sdim = 0; // stirng size

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // flag to zero
}

void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0; // flag to zero

    // carattere ricevuto messo nel buffer
    uart_buff_add();
    // incremento valore di toSend_char;
    toSend_char = toSend_char + 1;
}

void __attribute__((__interrupt__, _auto_psv)) _T2Interrupt(void) {
    LATGbits.LATG9 = (!LATGbits.LATG9);
    IFS0bits.T2IF = 0;
}

int main(void) {
    // analog pin set up 
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    // uart set up
    uart_setup(0); // interr fires when ANY char goes to the shif reg 

    // debug led set up
    TRISGbits.TRISG9 = 0;
    LATGbits.LATG9 = 0;
    
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 0;
    
    INTCON2bits.GIE = 1;

    srand(time(NULL)); // generazione numeri casuali
    int16_t length;

    while (1) {
        // create the string
        length = rand() % 30; // tra 0 e 29
        for (int16_t i = 0; i < length; i++) {
            toSend[i] = 'A';
        }
        // set string parameters
        sdim = strlen(toSend);
        if (sdim > 0){
            LATAbits.LATA0 = 1;
        }   
        head = 0;
        // activate the TX interrupt
        IEC0bits.U1TXIE = 1; // enable the TX interrupt
    }

    return 0;
}

// COMMIT 1 COMPORTAMENTO:
/*inizia con comportamento corretto, dopo aver inviato qualche carattere comincia a 
 stampare 2 caratteri per volta (caratteri extra sembrano essere in un ciclo con 
 * caratteri precedenti) err__if buff_empty settava ENABLE TX a zero*/

// COMMIT 2 COMPORTAMENTO:
/*comportamento apparentemente perfetto, viene ricevuto un char, inserito sul buffer
 circolare e poi sempre da li reinviato al TERMINALE. Nell'handler del TX c'è un led toggle
 per ogni esecuzione, se si inviano caratteri multipli può sembrare sbagliato è per via della 
 velocità di commutazione.
 PROVA: se il led è spento e si invia un numero di caratteri dispari il led si ACCENDE
        al contrario se si invia un numero di caratteri pari il led rimane spento(e viceversa)*/

// COMMIT 3 COMPORTAMENTO:



