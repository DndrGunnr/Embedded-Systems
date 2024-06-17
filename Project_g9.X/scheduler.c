/*
 * File:   scheduler.c
 * Author: enric
 *
 * Created on 12 giugno 2024, 12.05
 */


#include "xc.h"
#include "scheduler.h"

void scheduler(heartbeat schedInfo[], int nTasks) 
{
    int i;
    for (i = 0; i < nTasks; i++) {
        if (schedInfo[i].enable == 1) {
            schedInfo[i].n++;
            
            if( schedInfo[i].n >= schedInfo[i].N){ // periodic task 
                schedInfo[i].f(schedInfo[i].params);            
                schedInfo[i].n = 0;
                
                if(i == 1){
                    schedInfo[i].enable = 0; // disable the pwm stop task, and avoid n incrementation
                                             // in waiting mode
                }       
            }
        }
        
        
    }
}