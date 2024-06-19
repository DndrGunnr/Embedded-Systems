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

char rx_buffer[RX_DIM];
char tx_buffer[TX_DIM];
buffer rx_buffer_idx;
buffer tx_buffer_idx;




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
    buffer_init(&rx_buffer_idx);
    buffer_init(&tx_buffer_idx);

    return 1;
}

void save_char(char carattere){
    rx_buffer[rx_buffer_idx.tail] = carattere;
    rx_buffer_idx.tail++;
    if(rx_buffer_idx.tail >= RX_DIM){
        rx_buffer_idx.tail = 0;
    }
    if (rx_buffer_idx.tail == rx_buffer_idx.head)
        rx_buffer_idx.is_full = 1;
}

int16_t buffer_empty(int mode){
    int16_t temp = 0;
    switch(mode){
        case RX:
            if(rx_buffer_idx.head == rx_buffer_idx.tail && rx_buffer_idx.is_full == 0)
                temp = 1;
            break;
        default:
            if(tx_buffer_idx.head == tx_buffer_idx.tail && tx_buffer_idx.is_full == 0)
                temp = 1;
    }
    return temp;
}

int16_t get_buffer_length(buffer* buff){
    int length;
    //if the function is called without providing the argument, it is assumed the buffer can be retrieved
    //from the global scope of uart.c
    /*if (buff == NULL)
        buff=&rx_buffer_idx;*/

    if (buff->tail > buff->head)
        length = buff->tail - buff->head;
    else if (buff->tail == buff->head)
        if (buff->is_full == 0)
            length = 0;
        else
            length = MAX_COMMAND;
    else
        length = MAX_COMMAND - buff->head + buff->tail;
    return length;
}

void discard_command(){
    rx_buffer_idx.head = rx_buffer_idx.tail;
    rx_buffer_idx.is_full=0;
}

char get_char(){
    return rx_buffer[rx_buffer_idx.head];
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

        discard_command();
        // cannot put tail = head to avoid mixing different commands
        return 0;
    }
    return 1;

}


char* get_buffer(int mode){
    char* temp;
    switch(mode){
        case RX:
            temp = rx_buffer;
            break;
        default:
            temp = tx_buffer;
    }                 
    return temp;
}

int16_t get_buffer_head(int mode){
    int16_t temp;
    switch(mode){
        case RX:
            temp = rx_buffer_idx.head;
            break;
        default:
            temp = tx_buffer_idx.head;
    }  
    return temp;
}

void move_buffer_head(int mode) {
    switch (mode) {
        case RX:
            rx_buffer_idx.head++;
            rx_buffer_idx.is_full=0;
            if(rx_buffer_idx.head >= RX_DIM){
                rx_buffer_idx.head = 0;
            }
            break;
        default:
            tx_buffer_idx.head++;
            tx_buffer_idx.is_full=0;
            if(tx_buffer_idx.head >= TX_DIM){
                tx_buffer_idx.head = 0;
            }
    }  
}
  
void append_responce(int16_t type){
    switch(type){
        case COMM_GOOD:
            for(int16_t i = 0; i<AK_DIM; i++){
                tx_buffer[tx_buffer_idx.tail] = command_good_str[i];
                tx_buffer_idx.tail++;
                tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
                if (tx_buffer_idx.tail == tx_buffer_idx.head)
                    tx_buffer_idx.is_full = 1;
                
            }
            break;
        case COMM_BAD:
            for(int16_t i = 0; i<AK_DIM; i++){
                tx_buffer[tx_buffer_idx.tail] = command_bad_str[i];
                tx_buffer_idx.tail++;
                tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
                if (tx_buffer_idx.tail == tx_buffer_idx.head)
                    tx_buffer_idx.is_full = 1;                
            }
            break;
        case BATTERY:
            for(int16_t i = 0; i<HD_DIM; i++){
                tx_buffer[tx_buffer_idx.tail] = battery_header[i];
                tx_buffer_idx.tail++;
                tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
                if (tx_buffer_idx.tail == tx_buffer_idx.head)
                    tx_buffer_idx.is_full = 1;                
            }
            break;
        case IR:
            for(int16_t i = 0; i<HD_DIM; i++){
                tx_buffer[tx_buffer_idx.tail] = ir_header[i];
                tx_buffer_idx.tail++;
                tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
                if (tx_buffer_idx.tail == tx_buffer_idx.head)
                    tx_buffer_idx.is_full = 1;                
            }
            break;
        case MSG_END:
            tx_buffer[tx_buffer_idx.tail] = '*';
            tx_buffer_idx.tail++;
            tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
            if (tx_buffer_idx.tail == tx_buffer_idx.head)
                tx_buffer_idx.is_full = 1;            
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
        tx_buffer[tx_buffer_idx.tail] = number[i];
        tx_buffer_idx.tail++;
        tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
        if (tx_buffer_idx.tail == tx_buffer_idx.head)
            tx_buffer_idx.is_full = 1;        
        
        i++;
    }
    
}

void buffer_init(buffer* buff){
    buff->head = 0;
    buff->tail = 0;
    buff->is_full = 0;
}


void print_buff_log(int mode){
    char toSend[100];
    switch(mode){
        case RX:sprintf(toSend, "%d %d,", rx_buffer_idx.head, rx_buffer_idx.tail);break;
        default:sprintf(toSend, "%d %d,", tx_buffer_idx.head, tx_buffer_idx.tail); 
    }
    for(int16_t i = 0; i<strlen(toSend); i++){
        while (U1STAbits.UTXBF);
        U1TXREG = toSend[i];
    }
}

int16_t get_data_nuber(){
    int16_t temp = 0;
    temp = abs(rx_buffer_idx.tail - rx_buffer_idx.head);
    return temp;
}

