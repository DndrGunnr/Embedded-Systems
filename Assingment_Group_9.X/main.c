/*
 * File:   main.c
 * Author: enric
 *
 * Created on May 7, 2024, 4:37 PM
 */


#include <p33EP512MU810.h>


#include "xc.h"
#include "spi.h"
#include "timer.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "uart.h"

#define MAG_BUFF 5 // Number of Magnetometer mesurements to be read from the IMU
#define UART_BUFF 36 // Dimension of the message, in the worst case

// holder variable for the mesurements
int16_t x_sum;
int16_t y_sum;
int16_t z_sum;

// counter for the done mesurements
uint8_t counter;

// message variables
char toSend[UART_BUFF];
int head = 0;
int dim = 0;

void __attribute__((__interrupt__, __auto_psv__))_T2Interrupt(void) {
    // read the three MAG axes 
    x_sum =  spi_magRead(0x42,0x00F8,8);
    y_sum =  spi_magRead(0x44,0x00F8,8);
    z_sum =  spi_magRead(0x46,0x00FE,2);
    
    // inrcrease the number of mesurements done
    counter++;
    
    IFS0bits.T2IF=0;
    TMR2=0;
}

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; 
    
    while(U1STAbits.UTXBF == 0){ // until the TX trasmint buffer is full
        if(head >= dim){ // if the index is greater or equal to the current message dimension exit from the loop
            break;
        }else{
            U1TXREG = toSend[head]; // insert the first available char from the string to the TX trasmint buffer 
            head = head + 1; // increase the string index
        }
    }
}
    

int main(void) {
    //setup
    uart_setup(1, 0, 0, 0); //set uart interrupt to fire each time a char is transfered to the SHIFT register
    spi_setup(); //set spi frequency at 6MHz
    spi_magOn(); //set magnetometer to active mode at 25 Hz
    
    // variables initialization
    counter = 0;
    
    x_sum = 0;
    y_sum = 0;
    z_sum = 0;
    
    double x_avg;
    double y_avg;
    double z_avg;
    
    // IMU polling timer
    tmr_setup_period(TIMER2, 40);
    IEC0bits.T2IE = 1;
    
    // dead line check timer
    tmr_setup_period(TIMER3, 10);
    IEC0bits.T3IE = 0;
    
    uint8_t temp;

    while(1){
        algorithm(TIMER1, 7);
        
        if(counter == MAG_BUFF){ // five IMU polling effectuated
            // reset of index and counter 
            counter=0;
            head = 0;
            
            // compute the average of the last five mesurements
            x_avg = x_sum/MAG_BUFF;
            y_avg = y_sum/MAG_BUFF;
            z_avg = z_sum/MAG_BUFF;
            
            // compute the angle to the magnetic north
            double yaw = atan2(y_avg,x_avg);
            
            // create the actual message to be sent
            sprintf(toSend,"$MAG,%.0f,%.0f,%.0f,$YAW,%.2f",x_avg, y_avg, z_avg, yaw*(180.0/3.14));
            dim = strlen(toSend);
            
            x_sum=0;y_sum=0;z_sum=0;
            
            // forcefully activating the interrupt to start the trasmission
            IFS0bits.U1TXIF = 1;
        }  
        
        temp = tmr_wait_period_busy(TIMER3);
        if(temp){// if the dead line has been missed we exit from the while and return 0
            break;
        }else{
            continue;
        }
    }
   
    return 0;
}

