/*
 * File:   pwm.c
 * Author: enric
 *
 * Created on 7 giugno 2024, 21.56
 */


#include "xc.h"
#include "pwm.h"
#include "timer.h"


void pwm_setup() {
    ANSELD = 0x001E; // corresponding to binary 0000 000 0001 1110 that enable analog communication in pins RD1-4
    OC1CON1bits.OCTSEL = 0b111; //peripheral clock as source
    OC1CON1bits.OCM = 0b110; //edge aligned PWM
    OC1CON2bits.SYNCSEL = 0; //no sync source
    OC1RS = 7200; //pwm set to 10 KHz
    //set motor pins to output
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
}

void pwm_forward(int time) {
    OC1R = OC1RS * DUTY_CYCLE / 100;
    RPOR1bits.RP66R = PWM; //RP66 remapped to OC1
    RPOR2bits.RP68R = PWM; // RP68 remapped to OC1
    LATDbits.LATD1 = 0;
    LATDbits.LATD3 = 0;
}

void pwm_backward(int time) {
    OC1R = OC1RS * DUTY_CYCLE / 100;
    RPOR0bits.RP65R = PWM; //RP65 remapped to OC1
    RPOR1bits.RP67R = PWM; // RP67 remapped to OC1
    LATDbits.LATD2 = 0;
    LATDbits.LATD4 = 0;
}

void pwm_clockwise(int time) {
    OC1R = OC1RS * DUTY_CYCLE / 100;
    RPOR1bits.RP66R = PWM;
    RPOR1bits.RP67R = PWM;
    LATDbits.LATD1 = 0;
    LATDbits.LATD4 = 0;
}

void pwm_counter_clockwise(int time) {
    OC1R = OC1RS * DUTY_CYCLE / 100;
    RPOR0bits.RP65R = PWM;
    RPOR2bits.RP68R = PWM;
    LATDbits.LATD2 = 0;
    LATDbits.LATD3 = 0;
}

void pwm_stop(void){
    RPOR0bits.RP65R = 0;
    RPOR1bits.RP66R = 0;
    RPOR1bits.RP67R = 0;
    RPOR2bits.RP68R = 0;
}