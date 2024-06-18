/*
 * File:   main.c
 * Author: enric
 *
 * Created on June 11, 2024, 2:49 PM
 */


#include "xc.h"
#include "timer.h"
#include "uart.h"
#include "scheduler.h"
#include "parser.h"
#include "pwm.h"
#include "math.h"
#include "adc.h"

#define MAX_COMMAND 10

typedef struct{
    int x;
    int t;
}move;

typedef struct{
    int head;
    int tail;
    int is_full;
}buffer;

//states
int16_t is_waiting = 1; //wait status
int16_t is_moving = 0;

int16_t data_available = 0;//notify command to be converted
int16_t new_command = 0;

mesurements adc_value;

parser_state pstate;

void task_blink_led(void* param);
void task_battery_log(void *param);
void task_infraRed_log(void *param);
void task_move(void* param);
void scheduler_setup(heartbeat schedInfo[]);
void command_to_pwm(int);
int get_queue_length(buffer buff);

void __attribute__((__interrupt__, __no_auto_psv__)) _INT1Interrupt(){
    IFS1bits.INT1IF = 0;            // Reset button flag
    IEC1bits.INT1IE = 0;            // Disable button
    IEC0bits.T1IE = 1;              // Enable T2 interrupt
    tmr_setup_period(TIMER1, 30);
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T1Interrupt(){
    IFS0bits.T1IF = 0;     
    T1CONbits.TON = 0;      // Stop the timer
    IFS1bits.INT1IF = 0;    // Reset button flag
    IEC1bits.INT1IE = 1;    // Enable button
    
    if (PORTEbits.RE8==1) //value is 1 when button is not pressed
        is_waiting=(!is_waiting);
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(){
    IFS0bits.U1RXIF = 0;
    char new_char;
    
    while(U1STAbits.URXDA != 0){
        new_char = U1RXREG;
        save_char(new_char);
    }
    
    data_available = 1;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1TXInterrupt(){
    IFS0bits.U1TXIF = 0;
    char *faux_re = get_responce();
    
    while(U1STAbits.UTXBF == 0){ // until the TX trasmint buffer is full
        if(responce_empty()){ // if the index is greater or equal to the current message dimension exit from the loop
            break;
        }else{
            U1TXREG = faux_re[get_responce_head()]; // insert the first available char from the string to the TX trasmint buffer 
            move_responce_head(); // increase the string index
        }
    }
}

/*void __attribute__((__interrupt__, __no_auto_psv__)) _T3Interrupt(){
    TMR3=0;
    IFS0bits.T3IF=0;
    led_blink_counter++;
    if(led_blink_counter==1000){
        led_blink_counter=0;
        LATAbits.LATA0=(!LATAbits.LATA0);
        if(is_waiting){
            LATBbits.LATB8=(!LATBbits.LATB8);
            LATFbits.LATF1=(!LATFbits.LATF1);
        }   
    }
}*/

void setup(){
    ANSELA=ANSELB=ANSELC=ANSELD=ANSELE=ANSELG=0x0000;
    TRISEbits.TRISE8=1; //button for status toggle set as input
    TRISAbits.TRISA0=0; //ledA0 set to output
    RPINR0bits.INT1R=0x58; //interrupt1 remapped to interrupt
    INTCON2bits.GIE=1;
    IFS1bits.INT1IF = 0;
    IEC1bits.INT1IE=1;
    
    
    //turning lights pins
    TRISFbits.TRISF1=0;
    TRISBbits.TRISB8=0;
    //main loop frequency 1Khz
    tmr_setup_period(TIMER3, 1);
    IEC0bits.T3IE=0;
    //uart setup
    //int TX_interrupt_on, int TX_interrupt_type, int RX_interrupt_on, int RX_interrupt_type
    uart_setup(1,0,1,0);
    //pwm setup
    pwm_setup();
    //adc setup
    adc_setup();
    
    TRISGbits.TRISG9 = 0;
}

int main(void) {
    setup();
    //scheduler setup
    heartbeat schedInfo[MAX_TASKS];
    scheduler_setup(schedInfo);
    
    //parser setup
	pstate.state = STATE_DOLLAR;
	pstate.index_type = 0; 
	pstate.index_payload = 0;
    
    // variables for command conversion
    char *faux_pl;
    int16_t index_cm = 0;
    
    // command
    move command_queue[MAX_COMMAND];
    buffer buff;
    buff.head=0;
    buff.tail=0;
    buff.is_full=0;
    //state
    
    

    
    while(1){
        scheduler(schedInfo, MAX_TASKS);
        if(data_available == 1){
            for(int16_t i = 0; i<get_data_nuber(); i++){
                if(parse_byte(&pstate, get_char()) == NEW_MESSAGE){
                    //LATGbits.LATG9 = (!LATGbits.LATG9);
                    if(buff.is_full == 0){
                        if(parse_payload(pstate.msg_payload, pstate.index_payload)){
                            //LATGbits.LATG9 = (!LATGbits.LATG9);
                            faux_pl = pstate.msg_payload; // retrive the poiter to the payload buffer
                            // I want the position to the first char of the current command

                            command_queue[buff.tail].x = extract_integer(faux_pl); // send the extract integer with the payload buffer
                            // does not modify the indexes

                            index_cm = next_value(faux_pl, 0); // get the index to the start of the next integer
                            // next value cycles trought the vector(first arg), from the position of the index(second arg)

                            command_queue[buff.tail].t = extract_integer(faux_pl + index_cm); // extract the second integer

                            index_cm = next_value((faux_pl + index_cm), 0);

                            index_cm = 0;
                        }
                        
                        buff.tail = (buff.tail + 1) % MAX_COMMAND;
                        if (buff.tail == buff.head)
                            buff.is_full = 1;
                        
                        //print_buff_log();
                        //print_comm_log(command_queue_1.x, command_queue_1.t);
                        append_responce(COMM_GOOD);
                    }else{
                        append_responce(COMM_BAD);
                        discard_command();
                        /*for (int16_t i = 0; i < MAX_COMMAND; i++) {
                            print_comm_log(command_queue[i].x, command_queue[i].t);
                        }*/
                    }
                }
                
                
                //if(command_empty() == 1)
                    //break;
                if (command_empty()) {
                    //LATGbits.LATG9 = (!LATGbits.LATG9);
                    data_available = 0;
                }else{
                    move_command_head();
                }
                //print_buff_log();
            }
        }
        
        if (is_waiting == 0) {   
            if (is_moving == 0) {
                schedInfo[1].enable = 1;
                
                if (get_queue_length(buff) > 0) {
                    schedInfo[1].N = command_queue[ buff.head ].t; //set up the heartbeat period to stop pwm execution
                    command_to_pwm(command_queue[buff.head].x); //start motors
                    buff.head = (buff.head + 1) % MAX_COMMAND; //to account for wrap around
                    buff.is_full=0;
                    is_moving = 1;
                    //LATGbits.LATG9=is_moving;
                }
            }
        }else{
            if (is_moving == 1){
                /*task_move(&is_moving);
                schedInfo[1].enable = 0;
                schedInfo[1].n = 0;
                 */
                schedInfo[1].n = schedInfo[1].N;
            }
        }
      
        
        /*for(int16_t i = 0; i<buff.tail; i++){
            print_comm_log(command_queue[i].x,command_queue[i].t);
        }
         * 
         * if(command_1[queue_tail].t > 0){
            LATGbits.LATG9 = (!LATGbits.LATG9);
        }*/
            
        
        tmr_wait_period_busy(TIMER3);
    }
    
    
    
    /*schedInfo[0].N=1000;
    schedInfo[0].n=0;
    schedInfo[0].f=&task_blink_led;
    schedInfo[0].params= (void*)(&is_waiting);
    schedInfo[0].enable=1
    */
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------- TASK FUNCTION ----------------------------------------------------------- //
void task_blink_led(void* param){
    int* state = (int*) param;
    LATAbits.LATA0 = (!LATAbits.LATA0);
    if (*state) {
        LATBbits.LATB8 = (!LATBbits.LATB8);
        LATFbits.LATF1 = (!LATFbits.LATF1);
    }
}

void task_adc_sensing(void *param){
    mesurements* measure = (mesurements*) param;
    
    AD1CON1bits.DONE = 0;
    while(!AD1CON1bits.DONE);
    
    measure->battery = ADC1BUF0;
    measure->ir = ADC1BUF1;
}

void task_battery_log(void *param){
    double* adc_val = (double*) param;
    
    double ten_value = ((float)*adc_val/LV_CONV)*VDD;
            
    double battery = ten_value * 3;
    
    append_responce(BATTERY);
    append_number(battery, BATTERY);
    append_responce(MSG_END);
}

void task_infraRed_log(void *param){
   double* adc_val = (double*) param;
   
   double ten_value = ((float)*adc_val/LV_CONV)*VDD;
   
   double distance = (2.34 - 4.74 * ten_value + 4.06 * pow(ten_value, 2) - 1.60 * pow(ten_value, 3) + 0.24 * pow(ten_value, 4))*100;
   
   append_responce(IR);
   append_number(distance, IR);
   append_responce(MSG_END);
}

void task_move(void *param){
    int* is_moving = (int*) param;
    pwm_stop();
    *is_moving=0;
    //LATGbits.LATG9=*is_moving;
}

void scheduler_setup(heartbeat schedInfo[]){
    // led blink task
    schedInfo[0].N=1000;
    schedInfo[0].n=0;
    schedInfo[0].f=&task_blink_led;
    schedInfo[0].params= (void*)(&is_waiting);
    schedInfo[0].enable=1;
    
    //pwm stop scheduling
    schedInfo[1].N=0;
    schedInfo[1].n=0;
    schedInfo[1].f=&task_move;
    schedInfo[1].params=(void*)(&is_moving);
    schedInfo[1].enable=0;
    
    // adc sensing
    schedInfo[2].N=1;
    schedInfo[2].n=0;
    schedInfo[2].f=&task_adc_sensing;
    schedInfo[2].params=(void*)(&adc_value);
    schedInfo[2].enable=1;
    
    // IR logging
    schedInfo[3].N=100;
    schedInfo[3].n=-1;
    schedInfo[3].f=&task_infraRed_log;
    schedInfo[3].params=(void*)(&(adc_value.ir));
    schedInfo[3].enable=1;
   
    // battery logging 
    schedInfo[4].N=1000;
    schedInfo[4].n=-2;
    schedInfo[4].f=&task_battery_log;
    schedInfo[4].params=(void*)(&(adc_value.battery));
    schedInfo[4].enable=1;
}
// -------------------------------------------------------- TASK FUNCTION ----------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------- //

void command_to_pwm(int cmd){
    switch(cmd){
        case 1:
            pwm_forward();
            break;
        case 2:
            pwm_backward();
            break;
        case 3:
            pwm_clockwise();
            break;
        case 4:
            pwm_counter_clockwise();
    }
}

int get_queue_length(buffer buff) {
    int length;
    if (buff.tail > buff.head)
        length = buff.tail - buff.head;
    else if (buff.tail == buff.head)
        if (buff.is_full == 0)
            length = 0;
        else
            length = MAX_COMMAND;
    else
        length = MAX_COMMAND - buff.head + buff.tail;
    return length;
}