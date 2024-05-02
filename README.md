# Libraries

## Timers
### Macro:
* FCY: 72000000	---> clock frequency in one second 

* TIMER_LIMIT 65535 ---> max number of clock steps to store in a 16-bit register

* TIMER1 1
* TIMER2 2
* TIMER3 3
* TIMER4 4
* TIMER5 5
---
### Functions:

* **timer_setup_period()**: Needs as *input* the **timer** which you want to use and its **duration** in milliseconds. This function gives the possibility to choose in **adaptive** mode the **prescaler** of the timer and allow to **start** the **timer** as well as to **bring down** its **flag**.

* **tmr_counter_calculation()**: Needs as *input* the timer **duration** in milliseconds and the value of the **prescaler** (1, 8, 64, 256). This function compute the clocks steps which is compared within the *tmr_compute_prescaler()* function. This value tell us if it is too high to be put in a 16 bit register (because all timer is composed by a 16 bit register).

* **tmr_compute_prescaler()**: Needs as *input* the timer **duration** in milliseconds and then it calls the *tmr_counter_calculation()* function to get the value of the clock steps (**N.B**: clock steps < 65535) .

* **tmr_convert_prescaler()**: This function **transform** a **number** between 0 and 3 **to** a valid value of the **prescaler**.

* **tmr_wait_period()**: Needs as *input* the **timer**. This function **bring down** the **flag** of the timer and then **restart** the **timer**.

* **tmr_wait_ms()**:  Needs as *input* the **timer** which you want to use and its **duration** in milliseconds. This function setup the timer, then wait for it to expire and then stop the timer.

* **algotithm()**: Needs as *input* the **timer** which you want to use and its **duration** in milliseconds. This function is useful to simulate an execution in our code.

