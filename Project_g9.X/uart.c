/*
 * File:   uart.c
 * Author: Family
 *
 * Created on April 29, 2024, 7:59 AM
 */


#include "xc.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

char payload_buffer[RX_DIM];
int16_t head_pl = 0;
int16_t tail_pl = 0;


int uart_setup(int TX_interrupt_on, int TX_interrupt_type, int RX_interrupt_on, int RX_interrupt_type) {
    // UART SET UP

    RPINR18bits.U1RXR = 0x4b; // remapping the RX reg to the particular pin 
    RPOR0bits.RP64R = 1; // activating the pin of the TX reg

    // baude rate fixed to 9600 
    U1BRG = 468;

    U1MODEbits.UARTEN = 1; // enable uart COMMUNICATION
    U1STAbits.UTXEN = 1; // enable uart trasrmission 

    // TX reg INTERR type selection
    // possible TX_interrupt_type
    //0: interrupt when any char is transfered to the Transmit Shift Register
    //1: interrupt when the last character is removed from Transmit Shift Register
    //2: interrupt when the last char is transferred to the Transmit Shift register
    //3: RESERVED
    if (TX_interrupt_on) {
        IEC0bits.U1TXIE = 1; // enable the TX interrupt
        switch (TX_interrupt_type) {
            case 0:U1STAbits.UTXISEL1 = 0;U1STAbits.UTXISEL0 = 0;break;
               
            case 1:U1STAbits.UTXISEL1 = 0;U1STAbits.UTXISEL0 = 1;break;
                
            case 2:U1STAbits.UTXISEL1 = 1;U1STAbits.UTXISEL0 = 0;break;
                
            default:IEC0bits.U1TXIE = 0;break;
        }
    }
    
    // RX reg INTERR type selection
    //0 interrupt every byte received
    //2 buffer 3/4 full
    //3 receive buffer full
    if (RX_interrupt_on) {
        IEC0bits.U1RXIE = 1; // enable the TX interrupt
        switch (RX_interrupt_type) {
            case 0:U1STAbits.URXISEL1 = 0;U1STAbits.URXISEL0 = 0;break;
                
            case 2:U1STAbits.URXISEL1 = 1;U1STAbits.URXISEL0 = 0;break;
            
            case 3:U1STAbits.URXISEL1 = 1;U1STAbits.URXISEL0 = 1;break;
                
            default:IEC0bits.U1RXIE = 0;break;
        }
    }

    return 1;
}

void save_payload(char *payload, int16_t payload_dim){
    for(int16_t i = 0; i<payload_dim; i++){
        payload_buffer[tail_pl] = payload[i];
        tail_pl++;
    }
}

int16_t payload_empty(){
    int16_t temp = 0;
    if(tail_pl == head_pl){
        temp = 1;
    }
    return temp;
}

void move_payload_head(int16_t bytes){
    head_pl = head_pl + bytes;
    if(head_pl > RX_DIM){
        head_pl = 0;
    }
}

char *get_payload(){
    return payload_buffer;
}

int16_t get_payload_head(){
    return head_pl;
}

    