/* 
 * File:   timer.h
 * Author: utente
 *
 * Created on 28 maggio 2024, 15.47
 */

#include <xc.h> // include processor files - each processor file is guarded.

#define FCY 72000000
#define TIMER_LIMIT 65535
#define TIMER1 1
#define TIMER2 2
#define TIMER3 3
#define TIMER4 4
#define TIMER5 5

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

//function declarations
void tmr_setup_period(int timer, int ms);
int tmr_compute_prescaler(int ms);
float tmr_counter_calculation(int ms, int prescaler);
int tmr_convert_prescaler(int prescaler_bits);

int tmr_wait_period(int timer);
int tmr_wait_period_busy(int timer);
void tmr_wait_ms(int timer, int ms);
void algorithm(int timer, int ms);

