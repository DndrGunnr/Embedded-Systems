/*
 * File:   main.c
 * Author: enric
 *
 * Created on May 3, 2024, 5:06 PM
 */


#include "xc.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

#define CS LATDbits.LATD6 //shortcut for chipselector register

uint16_t LSB = 0;
uint16_t MSB = 0;

int main(void) {
    // disable analog pin
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    //debug led
    TRISAbits.TRISA0 = 0; // output
    LATAbits.LATA0=1;
    
    uart_setup(1);
    spi_setup();
    spi_magOn();
    
    
    while(1) {
        CS = 0;
        int8_t trash = spi_write(0x42 | read_mask);
        LSB = spi_write(0x00);
        MSB = spi_write(0x00);
        CS = 1;
        LSB = LSB & 0x00F8; //mask 0b00000011111000
        MSB = MSB << 8; //left-shift msb by 8
        MSB = MSB | LSB; //bit-wise OR with LSB
        MSB = MSB / 8;
        
        char toSend[100];
        sprintf(toSend, "$MAGX=%d", MSB);
        uart_send_string(toSend);
        
        tmr_wait_ms(TIMER1,100);
    }
    return 0;
}
