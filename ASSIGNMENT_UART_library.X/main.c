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

#define dim 10

char buffer[dim];


uint16_t elem_num_var;
uint16_t tail_var;
uint16_t head_var;

uint16_t new_char;
uint16_t *elem_num = &elem_num_var;
uint16_t *tail = &tail_var;
uint16_t *head = &head_var;


void __attribute__((__interrupt__, no_auto_psv__))_U1RXInterrupt(void){
    IFS0bits.U1RXIF = 0; // reset the flag of the RX reg
    
    //leggo nuovo elemento --> salvo elemento nel buffer
    new_char = 1;
    char temp = U1RXREG;
    buffer_add(buffer, temp, tail, elem_num, dim);
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
    
    // variable set up
    elem_num_var = 0;
    tail_var = 0;
    head_var = 0;
    
    //buffer_setup(buffer, dim);
    //char toSend[100];
    
    while(1){
        //every time 5 char are present in the buffer send the string 
        if(new_char == 1){
            send_char(buffer[head_var]);
            
            if(*elem_num > 0){
                LATGbits.LATG9 = 1; // start in OFF state
            }
            
            if(*head > 0){
                LATAbits.LATA0 = 1; // start in OFF state
            }
            
            /*sprintf(toSend, "%d,%d,%d", elem_num_var, head_var, tail_var);
            send_string(toSend);
            while(IFS0bits.U1TXIF != 1){}*/
            
            buffer_remove(buffer, head, elem_num, dim);
            new_char = 0;
            
            /*sprintf(toSend, "%d,%d,%d;", elem_num_var, head_var, tail_var);
            send_string(toSend);*/
        }
    }
    return 0;
}
