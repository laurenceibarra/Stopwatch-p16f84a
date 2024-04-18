/*
 * File:   newmain.c
 * Author: Lorens
 *
 * Created on April 16, 2024, 8:13 PM
 */

/*
 TODO : 
 * REENTRANCY
 * USE TMR0
 */

#include "config.h"

volatile char prev_RB7 = 0; // for saving the states for software edge triggering
volatile char curr_PORTB = 0;
volatile bool startFlag = 1;

void main(void) {
    OPTION_REGbits.INTEDG = 0;
    OPTION_REGbits.nRBPU = 0;
    INTCONbits.INTF = 0;
    INTCONbits.INTE = 1;
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = 1;
    INTCONbits.GIE = 1;
    
    TRISA = 0;
    TRISB = 0xff;
    PORTA = 0xff;
    curr_PORTB = PORTB;
    char prev_RB7 = (curr_PORTB & 10000000) >> 7;

    while(1) {
        if(!PORTBbits.RB3){
            for(int i = 0; i < 16; i++){
                __delay_ms(1000);
                PORTA--;
            }
        }
    }
    return;
}

void __interrupt() ISR() {
    INTCONbits.GIE = 0;
    if(INTCONbits.INTF && INTCONbits.INTE) {
        PORTA = 0xff;
        PORTB;
        NOP();
        INTCONbits.INTF = 0;
    }
    else if(INTCONbits.RBIF && INTCONbits.RBIE ){
        __delay_ms(50); //debouncing
        curr_PORTB = PORTB;
        char curr_RB7 = (curr_PORTB & 10000000) >> 7; //shift to the LSB
        
        //since our circuit is in high-low
        if(prev_RB7 == 1 && curr_RB7 == 0) {
            while(1){
                if(!PORTBbits.RB0) {
                    PORTA = 0xff;
                    __delay_ms(1000);
                }
                if(!PORTBbits.RB3){
                    INTCONbits.RBIF = 0;
                    curr_PORTB = PORTB;
                    prev_RB7 = (curr_PORTB & 10000000) >> 7;
                    INTCONbits.RBIF = 0;
                    return;
                }
            }
        }

        curr_PORTB = PORTB;
        prev_RB7 = (curr_PORTB & 10000000) >> 7;
        INTCONbits.RBIF = 0;
    }
    return;
}