/*
 * File:   Frequency_counter.c
 * Author: Emmanuel Esqueda Rodríguez
 *
 * Created on 30 de mayo de 2023, 06:31 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 4000000
#define Interrupts INTCONbits.GIE=1;
#define NoInterrupts INTCONbits.GIE=0;
#define T1ON T1CONbits.TMR1ON=1;
#define T1OFF T1CONbits.TMR1ON=0;
#define T0FC INTCONbits.TMR0IF=0;
#define T1FC PIR1bits.TMR1IF=0;

void display_show(void);
void calc(void);

unsigned char millon=0,cenmillar=0,decmillar=0,unimillar=0,centenas=0,decenas=0,unidades=0;
unsigned char display_numbers[] = {0x77, 0x41, 0x3B, 0x6B, 0x4D, 0x6E, 0x7E, 0x43, 0x7F, 0x6F};
unsigned char actual = 0;
unsigned char ready = 0;
unsigned long freq = 0, cuenta = 0;

void __interrupt() timers (void)
{
    if(PIR1bits.TMR1IF)
    {
        cuenta += 1;
        T1FC;
    }
    if(INTCONbits.TMR0IF){
        T1OFF;
		ready = 1;
        T0FC;
    }
}

void main(void) {
    INTCONbits.RBIF = 0;
	//OSCCON |= 0x70;
	//Configuracion del puerto A
	ANSEL = 0x00;
	TRISA = 0x00;
	PORTA = 0x00;
	//Configuracion del puerto B
	TRISB = 0x00;
	PORTB = 0x00;
	ANSELH = 0x00;
	//Timer 1
	T1CONbits.TMR1CS = 1; //SELECCION DE FUENTE EXTERNA
	T1CONbits.T1SYNC = 1; //NO SINCRONIZAR PULSOS
	TMR1H = 0x00;
	TMR1L = 0x00;
	//PARTE DE LAS INTERRUPCIONES DEL TMR1
	INTCONbits.PEIE = 1;//HABLITA LAS INTERRUPCIONES PERIFERICAS
	PIE1bits.TMR1IE = 1;//HABLITA INTERRUPCIONES EN EL OVERFLOW
	//Timer 0
	INTCONbits.T0IE = 1;
	T0FC;
	OPTION_REG = 0x86;
	//condiciones iniciales
	Interrupts;
	T0FC;
	T1ON;
	TMR0 = 0;
	while(1){
		if(ready == 1){
			ready = 0;
			calc();
			display_show();
			cuenta = 0;
			TMR1H = 0;
			TMR1L = 0;
			T0FC;
			Interrupts;
			T1ON;
			TMR0 = 0;
			
		}
	}
    
    return;
}

void display_show(void){
     for(actual = 0;actual <= 30;actual++){
          //
          PORTB = display_numbers[millon];
          PORTAbits.RA7 = 1;
          __delay_ms(2);
          PORTAbits.RA7 = 0;
          //
          PORTB = display_numbers[cenmillar];
          PORTAbits.RA6 = 1;
          __delay_ms(2);
          PORTAbits.RA6 = 0;
          //
          PORTB = display_numbers[decmillar];
          PORTAbits.RA5 = 1;
          __delay_ms(2);
          PORTAbits.RA5 = 0;
          //
          PORTB = display_numbers[unimillar];
          PORTAbits.RA4 = 1;
          __delay_ms(2);
          PORTAbits.RA4 = 0;
          //
          PORTB = display_numbers[centenas];
          PORTAbits.RA3 = 1;
          __delay_ms(2);
          PORTAbits.RA3 = 0;
          //
          PORTB = display_numbers[decenas];
          PORTAbits.RA2 = 1;
          __delay_ms(2);
          PORTAbits.RA2 = 0;
          //
          PORTB = display_numbers[unidades];
          PORTAbits.RA1 = 1;
          __delay_ms(2);
          PORTAbits.RA1 = 0;
     }
     return;
}

void calc(void){
    NoInterrupts;
    //Clean display data
	millon = 0;
    cenmillar = 0;
    decmillar = 0;
    unimillar = 0;
    centenas = 0;
    decenas = 0;
    unidades = 0;
    
	//Times that tmr1 get more than 65536
    freq = cuenta;
    freq = freq << 16;
	//tmr1 most significant
    cuenta = TMR1H;
    cuenta = cuenta << 8;
	//tmr1 lest significant
    cuenta = cuenta + TMR1L;
    freq = freq + cuenta;
	//conversion
    freq *= 61;
	freq >>= 1;
    
    while(freq >= 1000000){
        millon++;
        freq -= 1000000;
    }
    while(freq >= 100000){
        cenmillar++;
        freq -= 100000;
    }
    while(freq >= 10000){
        decmillar++;
        freq -= 10000;
    }
    while(freq >= 1000){
        unimillar++;
        freq -= 1000;
    }
    while(freq >= 100){
        centenas++;
        freq -= 100;
    }
    while(freq >= 10){
        decenas++;
        freq -= 10;
    }
    while(freq >= 1){
        unidades ++;
        freq --;
    }
    return;
}
