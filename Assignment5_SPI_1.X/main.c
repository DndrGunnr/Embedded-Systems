/*
 * File:   main.c
 * Author: enric
 *
 * Created on 18 aprile 2024, 12.23
 */


#include <p33EP512MU810.h>
#include "string.h"
#include "stdio.h"
#include "timer.h"
#include "spi.h"
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
    
    spi_setup();
    
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
    IEC0bits.U1TXIE = 0; // enable the TX interrupt
    
    // RX reg INTERR
    U1STAbits.URXISEL0 = 0; // RX interr set to trigger for every char recived
    U1STAbits.URXISEL1 = 0;
    
    IEC0bits.U1RXIE = 0; // RX interr enable
    
    //magnetometer setting to sleep mode->change bit 0 of register 0x4B to "1"
    LATDbits.LATD6=0; //set pin value to low to begin communication 
    uint8_t power_control_reg= 0x4B;
    uint8_t trash;
    trash=spi_write(power_control_reg);
    trash=spi_write(0x01);
    LATDbits.LATD6=1;
    tmr_wait_ms(TIMER1,2);
    
    //magnetometer setting to active mode -> change bit 1 & 2 of register 0x4C to "0" "0"
    LATDbits.LATD6=0;
    uint8_t OpMode_reg=0x4C;
    trash=spi_write(OpMode_reg);
    trash=spi_write(0x00);
    LATDbits.LATD6=1;
    tmr_wait_ms(TIMER1,2);    
    
    //chipId acquisition
    LATDbits.LATD6=0;
    uint8_t chipId_reg= 0x40;
    trash=spi_write(chipId_reg | read_mask);
    uint8_t chip_ID = spi_write(0x00); // get the value from the register
    LATDbits.LATD6=1; //put value of CS to 1 to end communication
    
    char toSend[100];
    sprintf(toSend,"%x", chip_ID);
    LATAbits.LATA0=1;
    send_string(toSend);

    
    while(1);
    
            
    return 0;
}
