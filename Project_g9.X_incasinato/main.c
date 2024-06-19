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

typedef struct{
    int x;
    int t;
}move;

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
    char *faux_re = get_buffer(TX);
    
    while(U1STAbits.UTXBF == 0){ // until the TX trasmint buffer is full
        if(buffer_empty(TX)){ // if the index is greater or equal to the current message dimension exit from the loop
            break;
        }else{
            U1TXREG = faux_re[get_buffer_head(TX)]; // insert the first available char from the string to the TX trasmint buffer 
            move_buffer_head(TX); // increase the string index
        }
    }
}


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
    move cmd_queue[MAX_COMMAND];
    buffer cmd_queue_idx;
    buffer_init(&cmd_queue_idx);

    while(1){
        scheduler(schedInfo, MAX_TASKS);
        if(data_available == 1){
            for(int16_t i = 0; i<get_data_nuber() ; i++){
                if(parse_byte(&pstate, get_char()) == NEW_MESSAGE){
                    if(cmd_queue_idx.is_full == 0){
                        if(parse_payload(pstate.msg_payload, pstate.index_payload)){
                            faux_pl = pstate.msg_payload; // retrive the poiter to the payload buffer
                            // I want the position to the first char of the current command

                            cmd_queue[cmd_queue_idx.tail].x = extract_integer(faux_pl); // send the extract integer with the payload buffer
                            // does not modify the indexes

                            index_cm = next_value(faux_pl, 0); // get the index to the start of the next integer
                            // next value cycles trought the vector(first arg), from the position of the index(second arg)

                            cmd_queue[cmd_queue_idx.tail].t = extract_integer(faux_pl + index_cm); // extract the second integer

                            index_cm = next_value((faux_pl + index_cm), 0);

                            index_cm = 0;
                        }
                        
                        cmd_queue_idx.tail = (cmd_queue_idx.tail + 1) % MAX_COMMAND;
                        if (cmd_queue_idx.tail == cmd_queue_idx.head)
                            cmd_queue_idx.is_full = 1;

                        //append_responce(COMM_GOOD);
                        //print_buff_log(RX);
                    }else{
                        //append_responce(COMM_BAD);
                        discard_command();
                        //print_buff_log();
                        /*for (int16_t i = 0; i < MAX_COMMAND; i++) {
                            print_comm_log(command_queue[i].x, command_queue[i].t);
                        }*/
                        //print_buff_log(RX);
                    }
                }
                if (buffer_empty(RX)) {
                    data_available = 0;
                }else{
                    move_buffer_head(RX);
                    
                }
                print_buff_log(RX);
            }
        }
        
        if (is_waiting == 0) {
           
            if (is_moving == 0) {
                schedInfo[1].enable = 1;
                
                if (get_buffer_length(&cmd_queue_idx) > 0) {
                    schedInfo[1].N = cmd_queue[ cmd_queue_idx.head ].t; //set up the heartbeat period to stop pwm execution
                    command_to_pwm(cmd_queue[cmd_queue_idx.head].x); //start motors
                    cmd_queue_idx.head = (cmd_queue_idx.head + 1) % MAX_COMMAND; //to account for wrap around
                    cmd_queue_idx.is_full=0;
                    is_moving = 1;
                }
            }
        }else{
            if (is_moving == 1){
                schedInfo[1].n = schedInfo[1].N;
            }
        }

        //print_buff_log(RX);
        tmr_wait_period_busy(TIMER3);
    }
    
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
    int16_t adc_batt; //raw battery value
    int16_t adc_ir; //raw ir value
    AD1CON1bits.DONE = 0;
    while(!AD1CON1bits.DONE);
    adc_batt = ADC1BUF0;
    adc_ir = ADC1BUF1;
    
    measure->battery = (((float)adc_batt/LV_CONV)*VDD)*VOLT_DIVIDER;
    measure->ir = (2.34 - 4.74 * ((float)adc_ir/LV_CONV)*VDD + 4.06 * pow(((float)adc_ir/LV_CONV)*VDD, 2) 
                    - 1.60 * pow(((float)adc_ir/LV_CONV)*VDD, 3) + 0.24 * pow(((float)adc_ir/LV_CONV)*VDD, 4))*100;
    
/*    if (measure->ir<=20){
        pwm_stop();
    }*/
}

void task_battery_log(void *param){
    double* battery = (double*) param;
    
    append_responce(BATTERY);
    append_number(*battery, BATTERY);
    append_responce(MSG_END);
}

void task_infraRed_log(void *param){
   double* distance = (double*) param;
   
   append_responce(IR);
   append_number(*distance, IR);
   append_responce(MSG_END);
}

void task_move(void *param){
    int* is_moving = (int*) param;
    pwm_stop();
    *is_moving=0;
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
    schedInfo[3].enable=0;
   
    // battery logging 
    schedInfo[4].N=1000;
    schedInfo[4].n=-2;
    schedInfo[4].f=&task_battery_log;
    schedInfo[4].params=(void*)(&(adc_value.battery));
    schedInfo[4].enable=0;
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

