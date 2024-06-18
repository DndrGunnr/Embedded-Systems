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
#include "math.h"

char command_good_str[AK_DIM] = "$MACK,1*";
char command_bad_str[AK_DIM]  = "$MACK,0*";
char battery_header[HD_DIM] = "$MBATT,";
char ir_header[HD_DIM] = "$MDIST,";

char command_buffer[RX_DIM];
int16_t head_cm = 0;
int16_t tail_cm = 0;

char responce_buffer[TX_DIM];
int16_t head_re = 0;
int16_t tail_re = 0;



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

void save_char(char carattere){
    command_buffer[tail_cm] = carattere;
    tail_cm++;
    tail_cm = tail_cm % RX_DIM;
}

int16_t command_empty(){
    int16_t temp = 0;
    if(head_cm == tail_cm){
        temp = 1;
    }
    return temp;
}

int16_t get_data_nuber(){
    int16_t temp = 0;
    temp = abs(tail_cm - head_cm);
    return temp;

}

void discard_command(){
    head_cm = tail_cm;
}

char get_char(){
    return command_buffer[head_cm];
}

void move_command_head(){
    head_cm++;
    if(head_cm >= TX_DIM){
        head_cm = 0;
    }
}

int16_t get_command_tail(){
    return tail_cm;
}

int16_t parse_payload(char *payload, int16_t payload_dim){
    int16_t wrong_comm = 0;
    
    for(int16_t i = 0; i<payload_dim; i++){
        if(payload[i] >= 48 && payload[i] <= 57){
            continue;
        }else{
            switch (payload[i]) {
                case ',': 
                    break; // go on
                case '\0': 
                    break; // go on
                default: wrong_comm = 1;
                    break; // discard command
            }
        }  
    }
    
    if(wrong_comm == 1){
        //LATGbits.LATG9 = 1;
        head_cm = tail_cm;
        // cannot put tail = head to avoid mixing different commands
        return 0;
    }
    return 1;

}




int16_t responce_empty(){
    int16_t temp = 0;
    if(tail_re == head_re){
        temp = 1;
    }
    return temp;
}

char *get_responce(){
    return responce_buffer;
}

int16_t get_responce_head(){
    return head_re;
}

void move_responce_head(){
    head_re++;
    //head_re = head_re % TX_DIM;
    if(head_re >= TX_DIM){
        head_re = 0;
    }
}
  
void append_responce(int16_t type){
    switch(type){
        case COMM_GOOD:
            for(int16_t i = 0; i<AK_DIM; i++){
                responce_buffer[tail_re] = command_good_str[i];
                tail_re++;
                tail_re = tail_re % TX_DIM;
            }
            break;
        case COMM_BAD:
            for(int16_t i = 0; i<AK_DIM; i++){
                responce_buffer[tail_re] = command_bad_str[i];
                tail_re++;
                tail_re = tail_re % TX_DIM;
            }
            break;
        case BATTERY:
            for(int16_t i = 0; i<HD_DIM; i++){
                responce_buffer[tail_re] = battery_header[i];
                tail_re++;
                tail_re = tail_re % TX_DIM;
            }
            break;
        case IR:
            for(int16_t i = 0; i<HD_DIM; i++){
                responce_buffer[tail_re] = ir_header[i];
                tail_re++;
                tail_re = tail_re % TX_DIM;
            }
            break;
        case MSG_END:
            responce_buffer[tail_re] = '*';
            tail_re++;
            tail_re = tail_re % TX_DIM;
    }
    // kick start the communication
    IFS0bits.U1TXIF = 1;
} 

void append_number(double value, int16_t type){
    char number[100];
    int16_t i = 0;
    switch(type){
        case BATTERY: 
            sprintf(number, "%.2f", value);
            break;
        case IR: 
            sprintf(number, "%d", (int16_t)value);
            break;
    }
    
    while(number[i] != '\0'){
        responce_buffer[tail_re] = number[i];
        tail_re++;
        tail_re = tail_re % TX_DIM;
        
        i++;
    }
    
}



// --------------------------------------------------------------- DEBUG MOMENTANEO ---------------------------------------------------------------//
void print_buff_log(){
    char toSend[100];
    sprintf(toSend, "%d %d,", head_cm, tail_cm);
    for(int16_t i = 0; i<strlen(toSend); i++){
        while (U1STAbits.UTXBF);
        U1TXREG = toSend[i];
    }
}

void print_comm_log(int16_t x, int16_t t){
    char toSend[100];
    sprintf(toSend, "%d %d,", x, t);
    for(int16_t i = 0; i<strlen(toSend); i++){
        while (U1STAbits.UTXBF);
        U1TXREG = toSend[i];
    }
}