/*
 * File:   timer.c
 * Author: Luca
 *
 * Created on April 29, 2024, 10:11 AM
 */


#include "xc.h"
#include "timer.h"

int tmr_compute_prescaler(int ms){
    if(tmr_counter_calculation(ms, 1) < TIMER_LIMIT){
        return 0;
    }
    else if(tmr_counter_calculation(ms, 8) < TIMER_LIMIT){
        return 1;
    }
    else if(tmr_counter_calculation(ms, 64) < TIMER_LIMIT){
        return 2;
    }
    else if(tmr_counter_calculation(ms, 256) < TIMER_LIMIT){
        return 3;
    }
    else{
        return 4;
    }
}

float tmr_counter_calculation(int ms, int prescaler){
    float return_value = (FCY / prescaler) * (ms / 1000.0);
    return return_value;
}

int tmr_convert_prescaler(int prescaler_bits){
    int return_value = 1;
    switch(prescaler_bits){
        case 0:
            return_value = 1;
            break;     
        case 1:
            return_value = 8;
            break;      
        case 2:
            return_value = 64;
            break;        
        case 3:
            return_value = 256;
            break;
    }
    
    return return_value;
}

void tmr_setup_period(int timer, int ms){
    int prescaler_bits;
    
    switch(timer){
        case 1:
            T1CONbits.TCS = 0; // set clock source to internal 72 MHz
    
            
            prescaler_bits = tmr_compute_prescaler(ms);
            T1CONbits.TCKPS = prescaler_bits; // set all the bits to 1
            
            PR1 = (FCY / tmr_convert_prescaler(prescaler_bits))*(ms/1000.0); 

            T1CONbits.TON = 1; // start the timer
            IFS0bits.T1IF = 0; // flag to zero
            break;
            
        case 2: 
            T2CONbits.TCS = 0; // set clock source to internal 72 MHz

            prescaler_bits = tmr_compute_prescaler(ms);
            T2CONbits.TCKPS = prescaler_bits; // set all the bits to 1
            
            PR1 = (FCY / tmr_convert_prescaler(prescaler_bits))*(ms/1000.0); 

            T2CONbits.TON = 1; // start the timer
            IFS0bits.T2IF = 0; // flag to zero            
            break;
            
        case 3:
            
            prescaler_bits = tmr_compute_prescaler(ms);
            T3CONbits.TCKPS = prescaler_bits; // set all the bits to 1
            
            PR3 = (FCY / tmr_convert_prescaler(prescaler_bits))*(ms/1000.0); 

            T3CONbits.TON = 1; // start the timer
            IFS0bits.T3IF = 0; // flag to zero 
            break;
            
        case 4:
            
            prescaler_bits = tmr_compute_prescaler(ms);
            T4CONbits.TCKPS = prescaler_bits; // set all the bits to 1
            
            PR4 = (FCY / tmr_convert_prescaler(prescaler_bits))*(ms/1000.0); 

            T4CONbits.TON = 1; // start the timer
            IFS1bits.T4IF = 0; // flag to zero 
            break;
            
        case 5:

            prescaler_bits = tmr_compute_prescaler(ms);
            T5CONbits.TCKPS = prescaler_bits; // set all the bits to 1

            PR5 = (FCY / tmr_convert_prescaler(prescaler_bits))*(ms / 1000.0);

            T5CONbits.TON = 1; // start the timer
            IFS1bits.T5IF = 0; // flag to zero 
            break;
    }
}

int tmr_wait_period(int timer){
    int ret_val = 0;
    
    switch(timer){
        case 1:
            ret_val = IFS0bits.T1IF;     // save flag value before reset
            while(IFS0bits.T1IF == 0);
            if(ret_val){
                IFS0bits.T1IF = 0;           // set flag bit to 0 --> flag goes to one when timer expire
                TMR1 = 0;                    // reset the timer and start watching the flag
            }    
            break;
            
        case 2:
            ret_val = IFS0bits.T2IF;    // save flag value before reset
            while(IFS0bits.T2IF == 0);
            if(ret_val){
                IFS0bits.T2IF = 0;          // set flag bit to 0 --> flag goes to one when timer expire
                TMR2 = 0;                   // reset the timer and start watching the flag
            }

            break;
            
        case 3:
            ret_val = IFS0bits.T3IF;    // save flag value before reset
            while(IFS0bits.T3IF == 0);
            if(ret_val){
                IFS0bits.T3IF = 0;          // set flag bit to 0 --> flag goes to one when timer expire
                TMR3 = 0;                   // reset the timer and start watching the flag
            }

            break;
            
        case 4:
            ret_val = IFS1bits.T4IF;    // save flag value before reset
            while(IFS1bits.T4IF == 0);
            if(ret_val){
                IFS1bits.T4IF = 0;          // set flag bit to 0 --> flag goes to one when timer expire
                TMR4 = 0;                   // reset the timer and start watching the flag
            }

            break;
            
        case 5:
            ret_val = IFS1bits.T5IF;    // save flag value before reset
            while(IFS1bits.T5IF == 0);
            if(ret_val){
                IFS1bits.T5IF = 0;          // set flag bit to 0 --> flag goes to one when timer expire
                TMR5 = 0;                   // reset the timer and start watching the flag
            }

            break;
    }
    return ret_val;
}

void tmr_wait_ms(int timer, int ms){
    int resto = ms;
        
    while((resto - 200) > 0){
        tmr_setup_period(timer, 200);
        switch(timer) {
            case 1:
                TMR1 = 0; // reset timer

                while (IFS0bits.T1IF == 0) { // wait timer termination
                    continue;
                }
                IFS0bits.T1IF = 0; // reset flag
                T1CONbits.TON = 0; // stop the timer
                break;
                
            case 2:
                TMR2 = 0; // reset timer

                while (IFS0bits.T2IF == 0) { // wait timer termination
                    continue;
                }
                IFS0bits.T2IF = 0; // reset flag
                T2CONbits.TON = 0; // stop the timer
                break;
                
            case 3:
                TMR3 = 0; // reset timer

                while (IFS0bits.T3IF == 0) { // wait timer termination
                    continue;
                }
                IFS0bits.T3IF = 0; // reset flag
                T3CONbits.TON = 0; // stop the timer
                break;
                
            case 4:
                TMR4 = 0; // reset timer

                while (IFS1bits.T4IF == 0) { // wait timer termination
                    continue;
                }
                IFS1bits.T4IF = 0; // reset flag
                T4CONbits.TON = 0; // stop the timer
                break;
                
            case 5:
                TMR5 = 0; // reset timer

                while (IFS1bits.T5IF == 0) { // wait timer termination
                    continue;
                }
                IFS1bits.T5IF = 0; // reset flag
                T5CONbits.TON = 0; // stop the timer
                break;
        }
        resto = resto - 200; // update the time
    }
    
    tmr_setup_period(timer, resto);
    switch(timer){
        case 1:
            TMR1 = 0; // reset timer
            
            while(IFS0bits.T1IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T1IF = 0; // reset flag
            T1CONbits.TON = 0; // stop the timer
            break;
            
        case 2:
            TMR2 = 0; // reset timer
            
            while(IFS0bits.T2IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T2IF = 0; // reset flag
            T2CONbits.TON = 0; // stop the timer
            break;
            
        case 3:
            TMR3 = 0; // reset timer
            
            while(IFS0bits.T3IF == 0){ // wait timer termination
                continue;
            }
            IFS0bits.T3IF = 0; // reset flag
            T3CONbits.TON = 0; // stop the timer
            break;
            
        case 4:
            TMR4 = 0; // reset timer
            
            while(IFS1bits.T4IF == 0){ // wait timer termination
                continue;
            }
            IFS1bits.T4IF = 0; // reset flag
            T4CONbits.TON = 0; // stop the timer
            break;
            
        case 5:
            TMR5 = 0; // reset timer
            
            while(IFS1bits.T5IF == 0){ // wait timer termination
                continue;
            }
            IFS1bits.T5IF = 0; // reset flag
            T5CONbits.TON = 0; // stop the timer
            break;
    }
}