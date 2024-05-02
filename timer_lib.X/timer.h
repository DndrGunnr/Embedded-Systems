/* 
 * File:   timer.h
 * Author: Luca
 *
 * Created on April 29, 2024, 10:11 AM
 */

// Macro declaration
#define FCY 72000000
#define TIMER_LIMIT 65535
#define TIMER1 1
#define TIMER2 2
#define TIMER3 3
#define TIMER4 4
#define TIMER5 5

//function declarations
void tmr_setup_period(int timer, int ms);
int tmr_compute_prescaler(int ms);
float tmr_counter_calculation(int ms, int prescaler);
int tmr_convert_prescaler(int prescaler_bits);

int tmr_wait_period(int timer);
void tmr_wait_ms(int timer, int ms);
void algorithm(int timer, int ms);
