#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define MAX_TASKS 5

typedef struct {
    int n;
    int N;
    int enable;
    void (*f)(void *);
    void* params;
} heartbeat;

void scheduler(heartbeat schedInfo[], int nTasks);

#endif	/* SCHEDULER_H */




