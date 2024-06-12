/*
 * File:   main.c
 * Author: enric
 *
 * Created on June 11, 2024, 2:49 PM
 */


#include "xc.h"
#include "timer.h"
#include "uart.h"

#define MAX_TASKS 1

typedef struct{
    int16_t n;
    int16_t N;
    int16_t enable;
}heartbeat;

int is_waiting=1; //wait status
int led_blink_counter=0; //in order to use the same timer for the main loop and the led blinking

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

void __attribute__((__interrupt__, __no_auto_psv__)) _U1TXInterrupt(){
    IFS0bits.U1TXIF = 0;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(){
    IFS0bits.U1RXIF = 0;
}

/*
 * void __attribute__((__interrupt__, __no_auto_psv__)) _T3Interrupt(){
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
}

 */

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
    tmr_setup_period(TIMER3, 1);
    IEC0bits.T3IE=0;
    //uart setup
    //int TX_interrupt_on, int TX_interrupt_type, int RX_interrupt_on, int RX_interrupt_type
    uart_setup(1,0,1,0);
}

void scheduler(heartbeat task_list[], int16_t state);
void task_blinkLed(int16_t state);

int main(void) {
    setup();
    heartbeat task_list[MAX_TASKS];
    
    task_list[0].n = 0;
    task_list[0].N = 1000;
    task_list[0].enable = 1;
    
    while(1){
        scheduler(task_list, is_waiting);
        tmr_wait_period(TIMER3);
    }
    
    
    return 0;
}

// -----------------------------------------------------Function definition------------------------------------------------------------------ //
void scheduler(heartbeat task_list[], int16_t state){
    for(int16_t i = 0; i < MAX_TASKS; i++){
        task_list[i].n++;
        if(task_list[i].enable == 1 && task_list[i].n == task_list[i].N){
            switch(i){
                case 0:
                    task_blinkLed(state);
                    break;
            }
            task_list[i].n = 0;
        }
    }
}


void task_blinkLed(int16_t state){
    LATAbits.LATA0=(!LATAbits.LATA0);
        if(state){
            LATBbits.LATB8=(!LATBbits.LATB8);
            LATFbits.LATF1=(!LATFbits.LATF1);
        }  
}