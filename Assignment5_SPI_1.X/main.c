/*
 * File:   main.c
 * Author: enric
 *
 * Created on 18 aprile 2024, 12.23
 */


#include <p33EP512MU810.h>
#include "string.h"
#include "stdio.h"
#include "xc.h"

uint16_t NEW_COMM;
uint16_t REC_CHAR;
uint16_t LED_BL_COUNTER; 

uint16_t COMM_INDX;
char COMMAND[5];
char RESET_STR[5] = {'\0'};


//functions for UART
void send_char(char carattere){
    U1TXREG = carattere;
}

void send_string(char *input_string){
    char toSend_str[100];
    
    sprintf(toSend_str, "%s", input_string);
    for (uint16_t i = 0; i < strlen(input_string); i++){
        send_char(toSend_str[i]);
    }
}

int main(void) {
    // disable analog pin
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    //debug led
    TRISAbits.TRISA0 = 0; // output
    

    
    //setup of pin for SPI connection
    TRISAbits.TRISA1 = 1; // RA1-RPI17 MISO
    TRISFbits.TRISF12 = 0; //RF12 -RP108 clock
    TRISFbits.TRISF13 = 0; // RF13-RP109 MOSI
    
    RPINR20bits.SDI1R = 0b0010001; //mapping pin SDI1 (MISO) to pin RPI17
    RPOR12bits.RP109R = 0b000101; //mapping SDO1 (MOSI) to pin RF13
    RPOR11bits.RP108R= 0b000110; //SPI clock
    //setup of registers for SPI specs
    SPI1CON1bits.MSTEN=1;// master mode
    SPI1CON1bits.MODE16=0; //8-bit mode 
    SPI1CON1bits.PPRE= 1; // 16:1
    SPI1CON1bits.SPRE= 0; // 8:1 -> resulting frequency is 562.5 KHz !!THIS IS A TRY!!
    SPI1CON1bits.CKP = 1; //idle clock for some reason
    SPI1STATbits.SPIEN= 1; //SPI enable
    //setup UART
    

    
    // UART SET UP
    RPINR18bits.U1RXR = 0x4b; // the input needs to be remapped to a particular pin
                              // as for the BUTTON to INTERR remapping (RX reg)
    RPOR0bits.RP64R = 1;      // on the other hand the output only needs to be "activated"
                              // since is a DATA output not a SINGNAL (TX reg)
    
    U1BRG = 468; // set baude rate
    U1MODEbits.UARTEN = 1; // enable uart COMMUNICATION --> similar to GEN ENAB
    U1STAbits.UTXEN = 1; // enable U1TX --> trasmission
    
    // TX reg INTERR
    U1STAbits.UTXISEL0 = 1; 
    U1STAbits.UTXISEL1 = 0; 
    // setting the TX interr to trigger when all the trasmission have occurred, than i can 
    // re enable the buttons
    IEC0bits.U1TXIE = 1; // enable the TX interrupt
    
    // RX reg INTERR
    U1STAbits.URXISEL0 = 0; // RX interr set to trigger for every char recived
    U1STAbits.URXISEL1 = 0;
    
    IEC0bits.U1RXIE = 1; // RX interr enable
    
    //magnetometer setting to sleep mode->change bit 0 of register 0x4B to "1"
    TRISCbits.TRISC1=0;
    uint16_t power_control_reg= 0x4B;
    while(SPI1STATbits.SPITBF==1);
    SPI1BUF= power_control_reg | 0x00; //setting MSB of register to "0" using bit-wise OR
    while(SPI1STATbits.SPIRBF==0); //waiting for something to show up
    uint16_t trash= SPI1BUF;
    while(SPI1STATbits.SPITBF==1);
    SPI1BUF=0x01;
    while(SPI1STATbits.SPIRBF==0);
    trash=SPI1BUF;
    
    //magnetometer setting to active mode -> change bit 1 & 2 of register 0x4C to "0" "0"
    uint16_t OpMode_reg=0x4C;
    while(SPI1STATbits.SPITBF==1);
    SPI1BUF= OpMode_reg | 0x00; //setting MSB of register to "0" using bit-wise OR
    while(SPI1STATbits.SPIRBF==0); //waiting for something to show up
    trash= SPI1BUF;
    while(SPI1STATbits.SPITBF==1);
    SPI1BUF=0x00;
    while(SPI1STATbits.SPIRBF==0);
    trash=SPI1BUF;
    
    //chipId acquisition
    uint16_t chipId_reg= 0x40;
    while (SPI1STATbits.SPITBF == 1);
    SPI1BUF = chipId_reg | 0x80; // setting the MSB to 1
    while (SPI1STATbits.SPIRBF == 0);
    trash = SPI1BUF; // read to prevent buffer overrun
    while (SPI1STATbits.SPITBF == 1);
    SPI1BUF = 0x00; // clocking out zeros so that the other chip can send the
    while (SPI1STATbits.SPIRBF == 0);
    uint16_t chip_ID = SPI1BUF; // get the value from the register
    TRISCbits.TRISC1 = 1;

    
    char toSend[100];
    sprintf(toSend, "roba: %d", chip_ID);
    send_string(toSend);
    
    while(1){
        LATAbits.LATA0=1;
    }
    
            
    return 0;
}
