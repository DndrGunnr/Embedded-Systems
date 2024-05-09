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


char buffer[uart_buff_dim];

uint16_t uart_elem_cnt;
uint16_t uart_tail;
uint16_t uart_head;

// possible TX_interrupt_type
    //0: interrupt when tx reg EMPTY
    //1: interrupt when all the transmit operations are completed
    //2: interrupt when any char is transferred to the Transmit Shift register
    //3: RESERVED
int uart_setup(int TX_interrupt_type){
    // UART SET UP
    
    RPINR18bits.U1RXR = 0x4b;// remapping the RX reg to the particular pin 
    RPOR0bits.RP64R = 1;     // activating the pin of the TX reg
    
    // baude rate fixed to 9600 
    U1BRG = 468;
    
    U1MODEbits.UARTEN = 1; // enable uart COMMUNICATION
    U1STAbits.UTXEN = 1; // enable uart trasrmission 
    
    // TX reg INTERR type selection
    if(TX_interrupt_type < 0 || TX_interrupt_type > 3){
        return 0;
    }else{
        switch(TX_interrupt_type){
            case 0:U1STAbits.UTXISEL1 = 0;U1STAbits.UTXISEL0 = 0;break;
            case 1:U1STAbits.UTXISEL1 = 0;U1STAbits.UTXISEL0 = 1;break;
            case 2:U1STAbits.UTXISEL1 = 1;U1STAbits.URXISEL0 = 0;break;
            default:break;
        }
        IEC0bits.U1TXIE = 0; // enable the TX interrupt
        // I want to manually enable the interrupt
    }
    
    // RX reg INTERR, fixed on interrupt on single char recived
    U1STAbits.URXISEL0 = 0; // RX interr set to trigger for every char recived
    U1STAbits.URXISEL1 = 0;
    IEC0bits.U1RXIE = 1;    // RX interr enable
    
    return 1;
}

int uart_is_buff_full(){
    if(uart_elem_cnt == uart_buff_dim){
        return 1;
    }
    return 0;
}
int uart_is_buff_empty(){
    if(uart_elem_cnt == 0){
        return 1;
    }
    return 0;
}

int uart_buff_add(){
    if(uart_is_buff_full()){
        return 0;// buffer is full, can't add any elements
                 // ADD SOME WAY TO DEBUG THIS ERROR
    }
    // save the char recived
    buffer[uart_tail] = U1RXREG;
    
    // update the index of the buffer
    uart_elem_cnt = uart_elem_cnt + 1 ;             // element counter
    uart_tail = (uart_tail + 1)%(uart_buff_dim);    // tail index

    return 1; // successfull operation return
}
int uart_buff_rmv(){
    if(uart_is_buff_empty()){
        return 0;// buffer is empty, can't remove any elements
    }
    
    // update the index of the buffer
    uart_elem_cnt = uart_elem_cnt - 1;              // element counter
    uart_head = (uart_head + 1)%(uart_buff_dim);    // head index

    return 1; // successfull operation return
}

void uart_send_head(){
    // print the char in the head position
    U1TXREG = buffer[uart_head];
}
void uart_send_char(char carattere){
    U1TXREG = carattere;
}
void uart_send_string(char *input_string){
    char toSend_str[100];
    
    sprintf(toSend_str, "%s", input_string);
    for (uint16_t i = 0; i < strlen(input_string); i++){
        uart_send_char(toSend_str[i]);
    }
}

void uart_log_buffer_info(){
    char toSend[100];
    // numero_elementi_nel_buffer, indice_testa, indice_coda
    sprintf(toSend, "%d,%d,%d", uart_elem_cnt, uart_head, uart_tail);
    uart_send_string(toSend);
}
