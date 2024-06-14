/*
 * File:   adc.c
 * Author: Family
 *
 * Created on 14 giugno 2024, 15.13
 */


#include "xc.h"
#include "adc.h"

void adc_setup(){
    AD1CON3bits.ADCS = 8; // ADC conversion clock  - Tad = 5 * TCY
    AD1CON3bits.SAMC = 16;// Auto sample time bits - 16 Tad
    
    AD1CON1bits.ASAM = 1; // Automatic sampling
    AD1CON1bits.SSRC = 7; // Automatic conversion
    
    AD1CON2bits.CSCNA = 1;// Scan mode activation
    
    AD1CON2bits.CHPS = 0; // Channel number selection
    AD1CHS0bits.CH0NA = 0;// Channel 0 negative input is VREFL
    
    ANSELBbits.ANSB14 = 1;// analog pin mode selection
    ANSELBbits.ANSB11 = 1;// activate AN5 AN11
    
    // dal file di dave+annika
    TRISBbits.TRISB11 = 1;
    TRISBbits.TRISB14 = 1;
    
    TRISBbits.TRISB9 = 1;
    LATBbits.LATB9 = 1;
    // dal file di dave+annika

    AD1CSSLbits.CSS5  = 1;// insert AN5 AN11 in the scan sequence
    AD1CSSLbits.CSS11 = 1;
    
    AD1CON2bits.SMPI = 1;// n of sequential operation before interrupt (if sequential is set)
    
    AD1CON1bits.ADON = 1;// ADC activation
}
