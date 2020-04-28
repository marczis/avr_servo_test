/*
 * File:   main.c
 * Author: marczis
 *
 * Created on April 9, 2020, 10:31 AM
 */

#include <xc.h>

//Config
//#include <avr/io.h>
#define F_CPU 1600000UL
#include <util/delay.h>

FUSES = {
	0xFF, // LOW{SUT_CKSEL=EXTXOSC_8MHZ_XX_16KCK_65MS, CKOUT=CLEAR, CKDIV8=CLEAR}
	0x98, // HIGH{BOOTRST=SET, BOOTSZ=2048W_3800, EESAVE=CLEAR, WDTON=CLEAR, SPIEN=SET, JTAGEN=SET, OCDEN=CLEAR}
	0xCB, // EXTENDED{BODLEVEL=2V6, HWBE=CLEAR}
};

LOCKBITS = {
	0xEF, // LOCKBIT{LB=NO_LOCK, BLB0=NO_LOCK, BLB1=SPM_DISABLE}
};

unsigned int motorA = 0;
unsigned int position = 62;

//TIFR = _BV(TOV0); - clear T0 OF IR flag
void __interrupt(TIMER0_OVF_vect_num) timer0isr(void) //CCI syntax
{
    //executed on every 16us
    //625 step is 10ms
    //62 1ms
    //124 2ms
    motorA += 1;
    if (motorA == 1250) {
        motorA = 0;
        //Set PIN high
        PORTE = 0x40; //PE6 high
    }
    if (motorA == position) { //16 
        //set PIN low
        PORTE = 0x00;
    }
    return;
}

int main(void) {
    //Set ports
    DDRB = 0xFF; //PD5 output
    DDRD = 0xFF;
    
    DDRE = 0x40; // PE6 output
    
    
    //Setup timer
    TCCR0B = 0x01; //clk for Timer TCNT0
    TIMSK0 = 0x01; // interrupt on overflow - 16us
    ei(); //Enable all interrupts
    //Main loop
    unsigned char direction = 0;
    while (1) {
        if (direction == 0) {
            position += 1;
        } else {
            position -= 1;
        }
        _delay_ms(500);

        _delay_ms(500);
        if (position == 150) {
            PORTB = 0xFE;
            PORTD = 0xFF;
            direction = 1; //124, 62
        }
        if (position == 40) {
            PORTD = 0xCF;
            PORTB = 0xFF;
            direction = 0;
        }
    }
    return 0;
}


/*
 PORTB |= _BV(PB1); Set bit
 EECR &= ~(_BV(EEPM4) | _BV(EEPM5)); Clear bit
 */