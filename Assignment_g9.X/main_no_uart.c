/*
 * File:   main.c
 * Author: enric
 *
 * Created on May 7, 2024, 4:37 PM
 */


#include <p33EP512MU810.h>


#include "xc.h"
//#include "uart.h"
#include "spi.h"
#include "timer.h"
#include "math.h"
#include "string.h"
#include "stdio.h"

#define mag_buff_dim 5
#define uart_buff_dim_tx 36 // circular buffer dimension
// aumentato a 35 per tenere conto dei segni '-', effetto: nessun effetto apparente 23.08
#define uart_buff_dim_rx 30

#define RX 1
#define TX 0

int16_t x_sum;
int16_t y_sum;
int16_t z_sum;
// PROBLEMA : variabile somme mai inizializzate
    // effetto inizializzazione globale: nessun effetto apparente 23.14
    // effetto inizializzazione nel main: nessun effetto apparente 23.16

uint8_t counter;
uint16_t trash;
int16_t LSB; //potrebbero creare conflitto --> tolti entrambi si è ciulata anche la x, z rimane ok
int16_t MSB; // avvicinando il telefono la x si sistema, seconda prova valori paiono casuali

char toSend[uart_buff_dim_tx];
int16_t head = 1;

// reinserite le definizioni come dei int16_t --> x, y ciulate; z ok

// PROBLEMA : variabile counter mai inizializzata
    // effetto inizializzazione globale: nessun effetto apparente 22.50
    // effetto inizializzazione nel main: nessun effetto apparente 22.55





void uart_send_char(char carattere) {
    //while (U1STAbits.UTXBF);
    U1TXREG = carattere;
}

void uart_send_string(char *input_string) {
    char toSend_str[100];

    strcpy(toSend_str, input_string);
    for (uint16_t i = 0; i < strlen(input_string); i++) {
        uart_send_char(toSend_str[i]);
    }
}

void __attribute__((__interrupt__, __auto_psv__))_T2Interrupt(void) {
    IEC0bits.T2IE = 0;
    x_sum = x_sum + spi_magRead(0x42,0x00F8,8);
    y_sum = y_sum + spi_magRead(0x44,0x00F8,8);
    z_sum = z_sum + spi_magRead(0x46,0x00FE,2);
    // utilizzo della sintassi estesa, effetto: nessun effetto apparente 23.10
    
    counter++;
    IFS0bits.T2IF=0;
    IEC0bits.T2IE=1;
    TMR2=0;
}

void algorithm(){
    tmr_wait_ms(TIMER1,7); 
}

int main(void) {
    //setup
    uart_setup(1); //set uart interrupt to fire each time a char is trans
    spi_setup(); //set spi frequency at 6MHz
    spi_magOn(); //set magnetometer to active mode at 25 Hz
    
    counter = 0;
    
    x_sum = 0;
    y_sum = 0;
    z_sum = 0;
    
    float x_avg;
    float y_avg;
    float z_avg;
    
    tmr_setup_period(TIMER2, 40, 1);
    
    TRISGbits.TRISG9 = 0; // set led to output

    while(1){
        if(IFS0bits.U1TXIF == 1){
            LATGbits.LATG9 = 1;
        }else{
            LATGbits.LATG9 = 0;
        }                   // notata la non accensione del led 22.53
                            // notata la mia idiozia --> led settato come input 23.33
                            // controllo con led bit SPIROV : nessuna accensione 23.36
                            // cambio con SPIROV == 0: led acceso (valori di x, y ancora fottuti)
        algorithm();
        if(counter == mag_buff_dim){
            counter=0;
            head = 1;
            
            x_avg = x_sum/mag_buff_dim;
            y_avg = y_sum/mag_buff_dim;
            z_avg = z_sum/mag_buff_dim;
            
            // cambio definizione dei valori di media: 
            
            double yaw = atan2(y_avg,x_avg);
            char toSend[uart_buff_dim_tx];
            // nota: divisione intera restituisce intero OK, mag_buff_dim non ha un TIPO in senso stretto
            //          viene sostituito con '5' a run time
            sprintf(toSend,"$MAG,%.0f,%.0f,%.0f,$YAW,%.2f",x_avg, y_avg, z_avg, yaw*(180.0/3.14));
            // riduzione del carico di dati da inviare, effetto: nessun effetto apparente 23.04 (reinserisco il valore di yaw)
            //uart_send_string(toSend);
            x_sum=0;y_sum=0;z_sum=0;
        }  
    }
   
    return 0;
}

// modifica definizione della funz-> spi_write, aggiunta variabile locale per il ritorno
    // effetto: nessun effetto apparente 00.01

// PROBLEMONE: funzione magRead ritorna un uint16_t modifica a valore di ritorno a int16_t
    // modificato anche i tipi degli argomenti tutti come int16_t
    // effetto: LESSSSSGOOOOOOOOOOOO valori in uscita sensati yeeeeee 00.17

// modifica: funizione atan2 richiede valori float come argomenti

