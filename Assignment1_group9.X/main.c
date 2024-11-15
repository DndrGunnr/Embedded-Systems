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
#include "string.h"
#include "stdio.h"

#define MAG_FRQ  4
#define AVG_WIND 5

int16_t x_sum;
int16_t y_sum;
int16_t z_sum;

uint8_t mag_poll;
uint8_t send_str;
uint16_t trash;
int16_t LSB; 
int16_t MSB; 

char toSend[UART_BUFF_DIM];
int head = 0;
int dim = 0;

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // flag to zero
    
    while(U1STAbits.UTXBF == 0){
        if(head >= dim){
            //LATAbits.LATA0 = 1;
            break;
        }else{
            U1TXREG = toSend[head];
            head = head + 1;
        }
    }
}
    

int main(void) {
    //setup
    uart_setup(1, 0, 0, 0); 
    //TX interrupt: ON, TYPE: last bit in TSR is sent; RX interrupt: OFF
    
    spi_setup(); //set spi frequency at 6MHz
    spi_magOn(); //set magnetometer to active mode at 25 Hz
    
    mag_poll = 0;
    send_str = 0;
    
    x_sum = 0;
    y_sum = 0;
    z_sum = 0;
    
    double x_avg;
    double y_avg;
    double z_avg;
    
    tmr_setup_period(TIMER2, 10);
    IEC0bits.T2IE = 0;
    
    TRISGbits.TRISG9 = 0; // set led to output
    TRISAbits.TRISA0 = 0; // set led to output
    
    uint8_t temp = 0;

    while(1){
        algorithm(TIMER1, 7);
        //LATGbits.LATG9 = (!LATGbits.LATG9);
        
        if(mag_poll == MAG_FRQ){
            x_sum = x_sum + spi_magRead(0x42,0x00F8,8);
            y_sum = y_sum + spi_magRead(0x44,0x00F8,8);
            z_sum = z_sum + spi_magRead(0x46,0x00FE,2);
            
            LATAbits.LATA0 = (!LATAbits.LATA0);
            
            mag_poll = 0;
            send_str++;
            
            if(send_str == AVG_WIND) {
            
                send_str = 0;
                head = 0;

                x_avg = x_sum/AVG_WIND;
                y_avg = y_sum/AVG_WIND;
                z_avg = z_sum/AVG_WIND;

                double yaw = atan2(y_avg,x_avg);

                sprintf(toSend,"$MAG,%.0f,%.0f,%.0f,$YAW,%.2f",x_avg, y_avg, z_avg, yaw*(180.0/3.14));
                dim = strlen(toSend);

                x_sum=0;y_sum=0;z_sum=0;

                // send first char to start interrupt calls
                U1TXREG = toSend[head];
                head = head + 1;
            }
        }
        
        mag_poll++;
        
        temp = tmr_wait_period_busy(TIMER2);
        //IFS0bits.T2IF = 0;
        if(temp){
            LATAbits.LATA0 = 1;
        }else{
            //LATAbits.LATA0 = 0;
        }
    }
    
    return 0;
}
