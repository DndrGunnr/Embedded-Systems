
#define PWM 0b010000
#define DUTY_CYCLE 70

#include <xc.h> // include processor files - each processor file is guarded.  

void pwm_forward(int);

void pwm_backward(int);

void pwm_clockwise(int);

void pwm_counter_clockwise(int);

void pwm_stop(void);

void pwm_setup(void);




