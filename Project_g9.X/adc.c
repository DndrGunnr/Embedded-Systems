/*
 * File:   adc.c
 * Author: Family
 *
 * Created on 14 giugno 2024, 15.13
 */

#include "xc.h"
#include "adc.h"

void adc_setup(){
    AD1CON3bits.ADCS = 8; // define TAD duration  
    AD1CON3bits.SAMC = 16;// define sampling time duration
    
    AD1CON1bits.ASAM = 1; // Automatic sampling
    AD1CON1bits.SSRC = 7; // Automatic conversion
    
    AD1CON2bits.CSCNA = 1;// Scan mode activation
    
    AD1CON2bits.CHPS = 0; // Channel number selection
    
    ANSELBbits.ANSB15 = 1;// analog pin mode selection
    ANSELBbits.ANSB11 = 1;// activate AN15 AN11
    
    TRISAbits.TRISA3 = 0;// ir enable pin
    LATAbits.LATA3 = 1;
    

    AD1CSSLbits.CSS11 = 1;// insert AN15 AN11 in the scan sequence
    AD1CSSLbits.CSS15  = 1;

    AD1CON2bits.SMPI = 1;// n of sequential operation before interrupt (if sequential is set)
    
    AD1CON1bits.ADON = 1;// ADC activation
}
