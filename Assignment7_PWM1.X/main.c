/*
 * File:   main.c
 * Author: enric
 *
 * Created on May 23, 2024, 12:23 PM
 */


#include "xc.h"
#include "timer.h"
#include "pwm.h"
int is_pwm_on=0;

void __attribute__((__interrupt__, __no_auto_psv__)) _INT1Interrupt(){
    
    IFS1bits.INT1IF = 0;            // Reset button flag
    IEC1bits.INT1IE = 0;            // Disable button
    IEC0bits.T1IE = 1;              // Enable T2 interrupt
    tmr_setup_period(TIMER1, 30);
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T1Interrupt(){
    
    IFS0bits.T1IF = 0;     
    T1CONbits.TON = 0;      // Stop the timer
    IFS1bits.INT1IF = 0;    // Reset button flag
    IEC1bits.INT1IE = 1;    // Enable button
    
    if (PORTEbits.RE8 == 1){   
        LATAbits.LATA0=1;
        pwm_forward(200);
        
            
    }
    LATAbits.LATA0=0;
            
}





int main(void) {
    ANSELA=ANSELB=ANSELC=ANSELD=ANSELE=ANSELG=0x0000;
    TRISAbits.TRISA0= 0; //LD1 debug led
    TRISEbits.TRISE8 = 1; //Button 1 as input
    RPINR0bits.INT1R=0x58; //interrupt1 remapped to interrupt
    IFS0bits.T1IF = 0;
    IFS1bits.INT1IF = 0;
    IEC1bits.INT1IE=1;
    //add PWM setup here
    ANSELD=0x001E; //corresponding to binary 0000 0000 0001 1110 activating RD1-4
    OC1CON1bits.OCTSEL=0b111; //peripheral clock as source
    OC1CON1bits.OCM=0b110; //edge aligned PWM
    OC1CON2bits.SYNCSEL=0b11111; //NO sync source
    OC1RS=7200;
    OC1R=7200;
    //set RD1-RD4 to output
    TRISDbits.TRISD1=0;
    TRISDbits.TRISD2=0;
    TRISDbits.TRISD3=0;
    TRISDbits.TRISD4=0;
    //remapping RD1 & RD3 to 0 (forward motion)
    LATDbits.LATD2=0;
    LATDbits.LATD4=0;
    
    
    while(1);
    
    return 0;
}
