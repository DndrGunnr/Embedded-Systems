/*
 * File:   scheduler.c
 * Author: Family
 *
 * Created on 12 giugno 2024, 12.26
 */


#include "xc.h"
#include "scheduler.h"

heartbeat task_list[MAX_TASKS];

void init_scheduler(){
    int16_t ni, Ni, enablei;
    
    for(int16_t i = 0; i < MAX_TASKS; i++){
        switch(i){
            case 0:
                ni = 0;
                Ni = 1000;
                enablei = 1;
                break;
        }
        
        task_list[i].n = ni;
        task_list[i].N = Ni;
        task_list[i].enable = enablei;
    }
}

void task_blinkLed(int16_t state){
    LATAbits.LATA0=(!LATAbits.LATA0);
        if(state){
            LATBbits.LATB8=(!LATBbits.LATB8);
            LATFbits.LATF1=(!LATFbits.LATF1);
        }  
}

void scheduler(int16_t state){
    for(int16_t i = 0; i < MAX_TASKS; i++){
        task_list[i].n++;
        if(task_list[i].enable == 1 && task_list[i].n == task_list[i].N){
            switch(i){
                case 0:
                    task_blinkLed(state);
                    break;
            }
            task_list[i].n = 0;
        }
    }
}



