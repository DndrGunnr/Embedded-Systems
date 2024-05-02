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

#define FCY 72000000 

// possible baude rate:
    //300, 600, 1200, 2400, 4800, 9600
    //14400, 19200, 28800, 38400, 56000
    //57600, 115200, 128000, 256000

// possible TX_interrupt_type
    //0: interrupt when tx reg EMPTY
    //1: interrupt when all the transmit operations are completed
    //2: interrupt when any char is transferred to the Transmit Shift register
    //3: RESERVED
int uart_setup(int TX_interrupt_type){
    // UART SET UP
    // this is fixed since we have a space problem on the car
    RPINR18bits.U1RXR = 0x4b; // the input needs to be remapped to a particular pin
                              // as for the BUTTON to INTERR remapping (RX reg)
    RPOR0bits.RP64R = 1;      // on the other hand the output only needs to be "activated"
                              // since is a DATA output not a SINGNAL (TX reg)
    
    // auto setting the baude rate
    U1BRG = 468;
    
    U1MODEbits.UARTEN = 1; // enable uart COMMUNICATION --> similar to GEN ENAB
    U1STAbits.UTXEN = 1; // enable U1TX --> trasmission
    
    // TX reg INTERR type selection
    if(TX_interrupt_type < 0 || TX_interrupt_type > 3){
        return 0;
    }else{
        switch(TX_interrupt_type){
            case 0:U1STAbits.UTXISEL0 = 0;U1STAbits.UTXISEL1 = 0;break;
            case 1:U1STAbits.UTXISEL0 = 1;U1STAbits.UTXISEL1 = 0;break;
            case 2:U1STAbits.UTXISEL0 = 0;U1STAbits.UTXISEL1 = 1;break;
            default:break;
        }
        IEC0bits.U1TXIE = 1; // enable the TX interrupt only if setted
    }
    
    // RX reg INTERR, fixed on interrupt on single data recived
    U1STAbits.URXISEL0 = 0; // RX interr set to trigger for every char recived
    U1STAbits.URXISEL1 = 0;
    
    IEC0bits.U1RXIE = 1; // RX interr enable
    
    return 1;
}

void buffer_setup(char *buff, uint16_t buff_size){
    buff = malloc(sizeof(char)*buff_size);
}
void buffer_destroy(char *buff){
    free(buff);
}

int is_buffer_full(uint16_t buff_size, uint16_t elem_num){
    if(elem_num == buff_size){
        return 1;
    }
    return 0;
}
int is_buffer_empty(uint16_t elem_num){
    if(elem_num == 0){
        return 1;
    }
    return 0;
}

int buffer_add(char *buff, char value, uint16_t *tail, uint16_t *elem_num, uint16_t size){
    if(is_buffer_full(size, *elem_num)){
        return 0;// buffer is full, can't add any elements
    }
    buff[*tail] = value;
    *elem_num = *elem_num + 1 ;
    *tail = (*tail + 1)%(size);
    // this line is supposed to reset the tail to 0 when size is reached
    return 1; // successfull operation return
}
int buffer_remove(char *buff, uint16_t *head, uint16_t *elem_num, uint16_t size){
    if(is_buffer_empty(*elem_num)){
        return 0;// buffer is full, can't remove any elements
    }
    *elem_num = *elem_num - 1 ;//remove the element
    *head = (*head + 1)%(size);
    // we can effectivaly leave the element in the buffer but ingoring it by moving 
    // the head index
    return 1; // successfull operation return
}

void send_char(char carattere){
    U1TXREG = carattere;
}
void send_string(char *input_string){
    char toSend_str[100];
    
    sprintf(toSend_str, "%s", input_string);
    for (uint16_t i = 0; i < strlen(input_string); i++){
        send_char(toSend_str[i]);
    }
}


