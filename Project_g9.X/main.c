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

typedef struct{
    int x;
    int t;
}move;

int16_t is_waiting = 1; //wait status
int16_t new_command = 0;//notify command to be converted

parser_state pstate;

void task_blink_led(void* param);
void scheduler_setup(heartbeat schedInfo[]);

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
    // after a char is recived, start the parsing
    new_char = U1RXREG;
    if(parse_byte(&pstate, new_char) == NEW_MESSAGE){
        //LATGbits.LATG9 = 1;
        
        // se riceviamo un nuovo messaggio
        // salviamo il payload in un buffer secondario
        save_payload(pstate.msg_payload, pstate.index_payload);
        // attiviamo la conversione
        new_command = 1;
    }
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1TXInterrupt(){
    IFS0bits.U1TXIF = 0;
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
    
    while(1){
        scheduler(schedInfo,MAX_TASKS);
        if(new_command){
            if(!payload_empty()){
                LATGbits.LATG9 = 1;
            }
        }
            
        
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

void scheduler_setup(heartbeat schedInfo[]){
    // led blink task
    schedInfo[0].N=1000;
    schedInfo[0].n=0;
    schedInfo[0].f=&task_blink_led;
    schedInfo[0].params= (void*)(&is_waiting);
    schedInfo[0].enable=1;
    
    // new command conversion and saving
    
    // battery sensing and logging 
    
    // IR logging
}
// -------------------------------------------------------- TASK FUNCTION ----------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------------------------------- //
