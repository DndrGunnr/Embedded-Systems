/*
 * File:   spi.c
 * Author: enric
 *
 * Created on 3 maggio 2024, 16.14
 */


#include "xc.h"


void spi_setup(){
    //setup of pin for SPI connection
    TRISAbits.TRISA1 = 1; // RA1-RPI17 MISO
    TRISFbits.TRISF12 = 0; //RF12 -RP108 clock
    TRISFbits.TRISF13 = 0; // RF13-RP109 MOSI
    TRISDbits.TRISD6=0; // chip select of magnetometer
    TRISBbits.TRISB3=0;
    TRISBbits.TRISB4=0;
    
    LATDbits.LATD6=1;
    LATBbits.LATB3=1;
    LATBbits.LATB4=1;
    
    RPINR20bits.SDI1R = 0b0010001; //mapping pin SDI1 (MISO) to pin RPI17
    RPOR12bits.RP109R = 0b000101; //mapping SDO1 (MOSI) to pin RF13
    RPOR11bits.RP108R= 0b000110; //SPI clock
    //setup of registers for SPI specs
    SPI1CON1bits.MSTEN=1;// master mode
    SPI1CON1bits.MODE16=0; //8-bit mode 
    SPI1CON1bits.PPRE= 0; // 64:1
    SPI1CON1bits.SPRE= 5; // 3:1 
    SPI1CON1bits.CKP = 1; //idle clock for some reason !!!
    SPI1STATbits.SPIEN= 1; //SPI enable    
}

int spi_write(unsigned int addr){
    while(SPI1STATbits.SPITBF); //waiting buffer to free up
    SPI1BUF=addr;
    while(SPI1STATbits.SPIRBF==0); //waiting buffer to receive something
    return SPI1BUF;
}
