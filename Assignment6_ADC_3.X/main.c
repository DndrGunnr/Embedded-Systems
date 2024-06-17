/*
 * File:   main.c
 * Author: Family
 *
 * Created on May 16, 2024, 12:12 PM
 */


#include <p33EP512MU810.h>

#include "xc.h"
#include "uart.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#define LV_CONV 1024
#define VDD 3.3

int16_t gl_index = 0;
int16_t gl_toSendLen = 0;
int16_t int_counter = 0;
char gl_toSend[4];


int16_t partitore = 3;
int16_t ADCBAT;
int16_t ADCIFR;
int16_t send_data = 0;

    

void __attribute__((__interrupt__, __no_auto_psv__))_U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; 
    
    while(U1STAbits.UTXBF == 0){ // until the TX trasmint buffer is full
        if(gl_index >= gl_toSendLen){ // if the index is greater or equal to the current message dimension exit from the loop
            break;
        }else{
            U1TXREG = gl_toSend[gl_index]; // insert the first available char from the string to the TX trasmint buffer 
            gl_index = gl_index + 1; // increase the string index
        }
    }
    
    //U1TXREG = 'K';
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T1Interrupt(void){
    send_data = 1;
    IFS0bits.T1IF = 0; // set timer flag to 0 --> to read next interrupt
    TMR1 = 0;           // reset timer
}

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0X0000;// analog pin disableing
    // NOTE
    /*
     sampling time = SAMC * TAD --> in automatic 
     
     */
    
    
    /*// ----------------------- ADC set up ---------------------
    AD1CON3bits.ADCS = 8; // ADC conversion clock select bits
    // set the speed of the conversion
    // selects how long is one Tad [1 Tcy - 64 Tcy]
    
    // exe 3 --> automatic conversion / automatic sampling
    // sampling mode
    AD1CON1bits.ASAM = 1;  
    
    // define conversion time (16 Tad)
    AD1CON3bits.SAMC = 16; // la conversione parte dopo 16*Tad ms
    
    // conversion mode (automatic)
    AD1CON1bits.SSRC = 7;
    
    // scan mode activation
    // attiva scan degli input
    AD1CON2bits.CSCNA = 1;
    
    // channel number selection
    AD1CON2bits.CHPS = 0; // single channel mode (reading only from CH0)
        // pag 33 del data sheet ADC
    
    // choose positive input to the channel (not used bacause scan mode is 1)
    //AD1CHS0bits.CH0SA = 5; // analog AN5 (numerazione progressiva)
                            // controllo sensore distanza
        // pag 14 del data sheet ADC
    
    // choose negative input to the channel
    AD1CHS0bits.CH0NA = 0;
    
    // choose negative input to the channel
    // AD1CHS123bits.CH123NA = 3; // stiamo settando gli input positivi di CH1,2,3
                               // che però non leggiamo data la scelta di CHPS
        // pag 13 del data sheet ADC
    
    // analog pin mode selection
    ANSELBbits.ANSB5  = 1;
    ANSELBbits.ANSB11 = 1;
        // pag 115 data sheet completo
    // insert the AN5 and AN11 in the scan input subset
    AD1CSSLbits.CSS5  = 1;
    AD1CSSLbits.CSS11 = 1;
    
    // SMPI settings
    AD1CON2bits.SMPI = 1;       // n of sequential operation before interrupt (if sequential is set)
    
    // ADC activation
    AD1CON1bits.ADON = 1;*/
    
    
    //AD1CON1bits.AD12B = 0;// set 10bit adc
    
    AD1CON3bits.ADCS = 8; // ADC conversion clock  - Tad = 5 * TCY
    AD1CON3bits.SAMC = 16;// Auto sample time bits - 16 Tad
    
    AD1CON1bits.ASAM = 1; // Automatic sampling
    AD1CON1bits.SSRC = 7; // Automatic conversion
    
    AD1CON2bits.CSCNA = 1;// Scan mode activation
    AD1CON2bits.CHPS = 0; // Channel number selection
    //AD1CHS0bits.CH0NA = 0;// Channel 0 negative input is VREFL
    
    ANSELBbits.ANSB14 = 1;// analog pin mode selection
    ANSELBbits.ANSB11 = 1;// activate AN5 AN11
    
    // dal file di dave+annika
    //TRISBbits.TRISB11 = 1;
    //TRISBbits.TRISB15 = 1;
    
    TRISBbits.TRISB9 = 0;//set ir enable 
    //LATAbits.LATA9 = 1;
    // dal file di dave+annika

    AD1CSSLbits.CSS11 = 1;
    AD1CSSLbits.CSS14 = 1;// insert AN5 AN11 in the scan sequence
    
    AD1CON2bits.SMPI = 1;// n of sequential operation before interrupt (if sequential is set)

    AD1CON1bits.ADON = 1;// ADC activation
    
    // debug led
    TRISGbits.TRISG9 = 0;
    TRISAbits.TRISA0 = 0;
    
    // uart set up
    uart_setup(1, 0, 0, 0);
    
    // setup timer
    //tmr_setup_period(TIMER1, 1000);
    //IEC0bits.T1IE = 1;
    
    // local variables
    double QV_bat, TN_bat;
    double QV_ifr, TN_ifr, CM_ifr;
    
    
    while(1){
        AD1CON1bits.DONE = 0;
        while (!AD1CON1bits.DONE);

        gl_index = 0;

        ADCBAT = ADC1BUF0;
        ADCIFR = ADC1BUF1;

        QV_bat = ((float)ADCBAT / LV_CONV)*VDD;
        QV_ifr = ((float)ADCIFR / LV_CONV)*VDD;

        // conversione in volt
        TN_bat = (QV_bat) * partitore;
        TN_ifr = QV_ifr;

        // conversione cm
        CM_ifr = (2.34 - 4.74 * TN_ifr + 4.06 * pow(TN_ifr, 2) - 1.60 * pow(TN_ifr, 3) + 0.24 * pow(TN_ifr, 4))*100;

        sprintf(gl_toSend, "%f %f,", TN_bat, CM_ifr);
        gl_toSendLen = strlen(gl_toSend);
        if (gl_toSendLen > 0) {
            LATGbits.LATG9 = (!LATGbits.LATG9);
        }

        IFS0bits.U1TXIF = 1;
        send_data = 0;
        
        tmr_wait_ms(TIMER1, 500);
    }
            
    return 0;
}


/*if(send_data){
            ADC_read();
            
            // quantizzazione
            LATAbits.LATA0 = (!LATAbits.LATA0);
            QV_bat = ADCBAT / lv_conv;
            QV_ifr = ADCIFR / lv_conv;
           
            // conversione in volt
            TN_bat = (QV_bat * volt)*partitore;
            TN_ifr = QV_ifr * volt;
            
            // conversione cm
            CM_ifr = (2.34 - 4.74 * TN_ifr + 4.06 * pow(TN_ifr, 2) - 1.60 * pow(TN_ifr, 3) + 0.24 * pow(TN_ifr, 4))*100;

            sprintf(gl_toSend, "%d", ADCBAT);
            gl_toSendLen = strlen(gl_toSend);
            if (gl_toSendLen > 0) {
                LATGbits.LATG9 = (!LATGbits.LATG9);
            }

            IFS0bits.U1TXIF = 1;
            send_data = 0;
        }  */