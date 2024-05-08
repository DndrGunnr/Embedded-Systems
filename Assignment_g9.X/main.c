/*
 * File:   main.c
 * Author: enric
 *
 * Created on May 7, 2024, 4:37 PM
 */


#include <p33EP512MU810.h>

#include "xc.h"
#include "uart.h"
#include "spi.h"
#include "timer.h"
#include "math.h"

#define mag_buff_dim 5

int16_t x_sum;
int16_t y_sum;
int16_t z_sum;
uint8_t counter;
uint16_t trash;
uint16_t LSB;
uint16_t MSB;

void __attribute__((__interrupt__, __auto_psv__))_T1Interrupt(void) {
    IEC0bits.T1IE = 0;
    x_sum+=spi_magRead(0x42,0x00F8,8);
    y_sum+=spi_magRead(0x44,0x00F8,8);
    z_sum+=spi_magRead(0x46,0x00FE,1);
    counter++;
    IFS0bits.T1IF=0;
    IEC0bits.T1IE=1;
    TMR1=0;
}

void algorithm(){
    tmr_wait_ms(TIMER1,7); 
}

int main(void) {
    //setup
    uart_setup(0); //set uart interrupt to fire each time a char is trans
    spi_setup(); //set spi frequency at 6MHz
    spi_magOn(); //set magnetometer to active mode at 25 Hz
    tmr_setup_period(TIMER1, 40, 1);
    
    while(1){
        algorithm();
        if(counter==mag_buff_dim){
            counter=0;
            int16_t yaw=atan2(y_sum/mag_buff_dim,x_sum/mag_buff_dim);
            char toSend[uart_buff_dim_tx];
            sprintf(toSend,"$MAG,%d,%d,%d,$YAW,%d",x_sum/mag_buff_dim, y_sum/mag_buff_dim, z_sum/mag_buff_dim, yaw);
            uart_send_string(toSend);
        }  
    }
   
    return 0;
}
