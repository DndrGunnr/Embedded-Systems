/*
 * File:   main.c
 * Author: Family
 *
 * Created on May 16, 2024, 12:12 PM
 */


#include "xc.h"
#include "uart.h"

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
    
    // conversion mode
    AD1CON1bits.SSRC = 0;
    
    // channel number selection
    AD1CON2bits.CHPS = 0; // single channel mode (reading only from CH0)
        // pag 33 del data sheet ADC
    
    // choose positive input to the channel
    AD1CHS0bits.CH0SA = 11; // analog AN11 (numerazione progressiva)
                            // controllo batteria
        // pag 14 del data sheet ADC
    // choose negative input to the channel
    AD1CHS123bits.CH123SA = 1; // stiamo settando gli input positivi di CH1,2,3
                               // che però non leggiamo data la scelta di CHPS
        // pag 13 del data sheet ADC
    
    // analog pin mode selection
    ANSELBbits.ANSB11 = 1;
        // pag 115 data sheet completo
    // SIMSAM e SMPI non vengono settati dato utilizzo single channel mode
    
    // ADC activation
    AD1CON1bits.ADON = 1;
    
    // debug led
    TRISGbits.TRISG9 = 0;
    TRISAbits.TRISA0 = 0;
    
    // BEGIN SAMPLING --> set sampling bit to 1
    
    
            
    
    while(1){
        LATGbits.LATG9 = 1;
    }
            
    return 0;
}
