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

#define RX_DIM 200
#define TX_DIM 200
#define TX 1
#define RX 2
#define AK_DIM 8 
#define HD_DIM 7
#define COMM_GOOD 1
#define COMM_BAD  2
#define BATTERY   3
#define IR        4
#define MSG_END   5
#define MAX_COMMAND 10


// set all UART mode, pin, interrupts ecc
int16_t uart_setup(int16_t TX_interrupt_on, int16_t TX_interrupt_type, int16_t RX_interrupt_on, int16_t RX_interrupt_type); 

void save_char(char carattere);

int16_t buffer_empty(int mode);

void discard_command();

char get_char();

int16_t parse_payload(char *payload, int16_t payload_dim);

int16_t get_buffer_length(buffer*);

void append_responce(int16_t type);
void append_number(double value, int16_t type);

int16_t get_buffer_head(int);
char *get_buffer(int);
void move_buffer_head(int);


void buffer_init(buffer*);


void print_buff_log(int mode);

int16_t get_data_nuber();