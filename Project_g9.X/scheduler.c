/*
 * File:   scheduler.c
 * Author: enric
 *
 * Created on 12 giugno 2024, 12.05
 */


#include "xc.h"
#include "scheduler.h"
#include "adc.h"
#include "math.h"

void scheduler(heartbeat schedInfo[], int nTasks) 
{
    int i;
    for (i = 0; i < nTasks; i++) {
        if (schedInfo[i].enable == 1) {
            schedInfo[i].n++;
            
            if( schedInfo[i].n >= schedInfo[i].N){ // scheduling of periodic task 
                schedInfo[i].f(schedInfo[i].params);            
                schedInfo[i].n = 0;
                
                // to stop the command execution if state goes in waitng mode before the command is complete
                // we need to disable the "task_stop", to avoid incrementing n in waiting mode  
                if(i == 1){
                    schedInfo[i].enable = 0; 
                }       
            }
        }
        
        
    }
}