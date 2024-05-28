/*
 * File:   main.c
 * Author: Family
 *
 * Created on May 16, 2024, 12:12 PM
 */


#include "xc.h"
#include "uart.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

int16_t gl_index = 0;
int16_t gl_sampl = 1;
int16_t gl_toSendLen = 0;
char gl_toSend[4];
float lv_conv = 1024.0;
float volt = 1.7;
    

void __attribute__((__interrupt__, no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; 
    
    while(U1STAbits.UTXBF == 0){ // until the TX trasmint buffer is full
        if(gl_index >= gl_toSendLen){ // if the index is greater or equal to the current message dimension exit from the loop
            gl_sampl = 1;
            break;
        }else{
            U1TXREG = gl_toSend[gl_index]; // insert the first available char from the string to the TX trasmint buffer 
            gl_index = gl_index + 1; // increase the string index
        }
    }
}

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;// analog pin disableing
    // NOTE
    /*
     sampling time = SAMC * TAD --> in automatic 
     
     */
    
    
    // ----------------------- ADC set up ---------------------
    AD1CON3bits.ADCS = 8; // ADC conversion clock select bits
    // set the speed of the conversion
    // selects how long is one Tad [1 Tcy - 64 Tcy]
    
    // exe 1 --> manual conversion / manual sampling
    // sampling mode
    AD1CON1bits.ASAM = 0;
    
    // conversion mode (automatic)
    AD1CON1bits.SSRC = 7;
    
    // define conversion time (16 Tad)
    AD1CON3bits.SAMC = 16; // la conversione parte dopo 16*Tad ms
    
    // channel number selection
    AD1CON2bits.CHPS = 0; // single channel mode (reading only from CH0)
        // pag 33 del data sheet ADC
    
    // choose positive input to the channel
    AD1CHS0bits.CH0SA = 5; // analog AN5 (numerazione progressiva)
                            // controllo sensore distanza
        // pag 14 del data sheet ADC
    // choose negative input to the channel
    AD1CHS123bits.CH123SA = 1; // stiamo settando gli input positivi di CH1,2,3
                               // che però non leggiamo data la scelta di CHPS
        // pag 13 del data sheet ADC
    
    // analog pin mode selection
    ANSELBbits.ANSB5 = 1;
        // pag 115 data sheet completo
    // SIMSAM e SMPI non vengono settati dato utilizzo single channel mode
    
    // ADC activation
    AD1CON1bits.ADON = 1;
    
    // debug led
    TRISGbits.TRISG9 = 0;
    TRISAbits.TRISA0 = 0;
    
    // uart set up
    uart_setup(1, 0, 0, 0);
    
    // BEGIN SAMPLING --> set sampling bit to 1
    
    int16_t ADCValue;
    double QUANValue;
    double TENValue;
    double METERValue;

    
    while(1){
        if(gl_sampl == 1){
            gl_index = 0;
            gl_sampl = 0;
            
            AD1CON1bits.SAMP = 1; // Start sampling
            // AD1CON1bits.SAMP = 0; // Automatic conversion
            while (!AD1CON1bits.DONE); // Wait for the conversion to complete
            AD1CON1bits.DONE = 0;       // clear done status
            ADCValue = ADC1BUF0;
            
            // quantizzazione
            QUANValue = ADCValue/lv_conv;
            
            // conversione in volt
            TENValue = QUANValue*volt;
            
            // conversione in metri
            METERValue = 2.34 - 4.74*TENValue + 4.06*pow(TENValue, 2) - 1.60*pow(TENValue, 3) + 0.24*pow(TENValue, 4);
            
            sprintf(gl_toSend, "%.5f m", TENValue);
            gl_toSendLen = strlen(gl_toSend);
            if(gl_toSendLen > 0){
                LATGbits.LATG9 = (!LATGbits.LATG9);
            }
            
            IFS0bits.U1TXIF = 1;
        }
        
        tmr_wait_ms(TIMER2, 400);
    }
            
    return 0;
}
