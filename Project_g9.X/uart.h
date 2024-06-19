/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "stdio.h"
#include "string.h"
#include "timer.h"

// circular buffer dimension
#define RX_DIM 200
#define TX_DIM 80

// circular buffer id
#define TX 1
#define RX 2

// default message dimension
#define AK_DIM 8 
#define HD_DIM 7

// item identifier
#define COMM_GOOD 1
#define COMM_BAD  2
#define BATTERY   3
#define IR        4
#define MSG_END   5
#define MAX_COMMAND 10

// buffer indexeses structure
typedef struct{
    int head;
    int tail;
    int is_full;
}buffer;

// set all UART mode, pin, interrupts 
int16_t uart_setup(int16_t TX_interrupt_on, int16_t TX_interrupt_type, int16_t RX_interrupt_on, int16_t RX_interrupt_type); 

// function used to save the argument char in the RX circular buffer
void save_char(char);

// return 0 if the choosen buffer is empty, 1 otherwise
int16_t buffer_empty(int);

// eliminate the last recived command from the RX buffer 
void discard_command();

// returns the char in the head position for the RX circular buffer
char get_char();

// returns 1 if the payload of a message contains only accettable characters
int16_t parse_payload(char*, int16_t);

// returns the number of elements in the given buffer
int16_t get_buffer_length(buffer*);

// used to insert responce in the TX buffer
void append_responce(int16_t);

// used to insert numeric value in the TX buffer
void append_number(double, int16_t);

// returns the value of the buffer choosen by the arg
int16_t get_buffer_head(int);

// returns the pointer to the buffer choosen by the arg
char *get_buffer(int);

// increments by one the head index of the buffer choosen by the arg
void move_buffer_head(int);

// initialize the value of the buffer instance ginven as arg
void buffer_init(buffer*);

// returns the number of char in the RX buffer
int16_t get_data_count();