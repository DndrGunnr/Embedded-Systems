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

// fixed message
char command_good_str[AK_DIM] = "$MACK,1*";
char command_bad_str[AK_DIM]  = "$MACK,0*";
char battery_header[HD_DIM] = "$MBATT,";
char ir_header[HD_DIM] = "$MDIST,";

// circular buffer definition
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
    // circular buffer initialization
    buffer_init(&rx_buffer_idx);
    buffer_init(&tx_buffer_idx);

    return 1;
}

void save_char(char carattere){
    rx_buffer[rx_buffer_idx.tail] = carattere; // save arg in head position of the RX buffer
    rx_buffer_idx.tail++; // increase head index
    
    if(rx_buffer_idx.tail >= RX_DIM){ // head index circle back control
        rx_buffer_idx.tail = 0;
    }
    
    if (rx_buffer_idx.tail == rx_buffer_idx.head) // control if the buffer is full, head == tail but data have not been consumed
        rx_buffer_idx.is_full = 1;
}

int16_t buffer_empty(int mode){
    int16_t temp = 0;
    switch(mode){
        case RX:
            // EMPTY : head == tail, is_full == 0
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
    //------------------------------cancellare----------------------------------
    //if the function is called without providing the argument, it is assumed the buffer can be retrieved
    //from the global scope of uart.c
    /*if (buff == NULL)
        buff=&rx_buffer_idx;*/
    //------------------------------cancellare----------------------------------
    
    if (buff->tail > buff->head) 
        // default case: tail > head, number of elements = tail - head
        length = buff->tail - buff->head;
    else if (buff->tail == buff->head)
        // buffer full or empty: tail == head but
        if (buff->is_full == 0)
            length = 0; // is_full == 0, no data present
        else
            length = MAX_COMMAND; // is_full == 1, all data have not been consumed
    else
        // buffer is not full, but head > tail
        length = MAX_COMMAND - buff->head + buff->tail;
    return length;
}

void discard_command(){
    // after reciveing a command, we can put the indexes equal to each other.
    // the last command will not undergo parsing, and will be discarted
    rx_buffer_idx.head = rx_buffer_idx.tail;
    rx_buffer_idx.is_full=0;
}

char get_char(){
    return rx_buffer[rx_buffer_idx.head];
}

int16_t parse_payload(char *payload, int16_t payload_dim){
    int16_t wrong_comm = 0;
    
    // the only char allowed in the payload are digits from 0 to 9, and ',' '\0'
    if(payload[0] >= 49 && payload[0] <= 52){ // char in first position has to be a digit from 1 to 4
        if(payload[1] == ','){ // second char has to be the ','
            for (int16_t i = 2; i < (payload_dim); i++) {
                if(payload[i] >= 48 && payload[i] <= 57){ // after that the char can be eithe a digiti (0 to 9) to the '\0'
                    continue; 
                }else if(payload[i] == '\0'){
                    continue;
                }else{
                    wrong_comm = 1;
                }
            }
        }
    }else{
        wrong_comm = 1;
    }
    
    // if the payload is wrong i discard the last message recived
    if(wrong_comm == 1){
        discard_command();
        return 0;
    }
    return 1;

}

// return the choosen buffer pointer
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

// return the value of the head index for the choosen buffer
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
            rx_buffer_idx.head++; // consume the char in the head position
            rx_buffer_idx.is_full=0; // the buffer is now not full
            
            if(rx_buffer_idx.head >= RX_DIM){ // index circle back control
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
        case COMM_GOOD: // insert "$MACK,1* in the TX buffer
            for(int16_t i = 0; i<AK_DIM; i++){
                tx_buffer[tx_buffer_idx.tail] = command_good_str[i];
                tx_buffer_idx.tail++;
                tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
                if (tx_buffer_idx.tail == tx_buffer_idx.head)
                    tx_buffer_idx.is_full = 1;
                
            }
            break;
        case COMM_BAD: // insert "$MACK,0* in the TX buffer
            for(int16_t i = 0; i<AK_DIM; i++){
                tx_buffer[tx_buffer_idx.tail] = command_bad_str[i];
                tx_buffer_idx.tail++;
                tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
                if (tx_buffer_idx.tail == tx_buffer_idx.head)
                    tx_buffer_idx.is_full = 1;                
            }
            break;
        case BATTERY: // insert "$MBATT," in the TX buffer, header of the message
            for(int16_t i = 0; i<HD_DIM; i++){
                tx_buffer[tx_buffer_idx.tail] = battery_header[i];
                tx_buffer_idx.tail++;
                tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
                if (tx_buffer_idx.tail == tx_buffer_idx.head)
                    tx_buffer_idx.is_full = 1;                
            }
            break;
        case IR: // insert "$MDIST," in the TX buffer, header of the message
            for(int16_t i = 0; i<HD_DIM; i++){
                tx_buffer[tx_buffer_idx.tail] = ir_header[i];
                tx_buffer_idx.tail++;
                tx_buffer_idx.tail = tx_buffer_idx.tail % TX_DIM;
                if (tx_buffer_idx.tail == tx_buffer_idx.head)
                    tx_buffer_idx.is_full = 1;                
            }
            break;
        case MSG_END:// insert '+' in the TX buffer, termination of the message
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
    // convert the args value in char form and append it to the TX buffer
    char number[100];
    int16_t i = 0;
    switch(type){ // differentiate for the message form 
        case BATTERY: 
            sprintf(number, "%.2f", value); // precision 2 for the battery
            break;
        case IR: 
            sprintf(number, "%d", (int16_t)value); // integer for distance
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


int16_t get_data_count(){
    int16_t temp = 0;
    temp = abs(rx_buffer_idx.tail - rx_buffer_idx.head);
    return temp;
}

