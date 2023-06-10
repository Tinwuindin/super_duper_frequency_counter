# super_duper_frequency_counter

This is a frequency counter that works with square wave frequencies ranging from 100Hz to 9MHz and offers a high level of precision.
The program was written in MPLAB X IDE using C90.

The program works with Timer 1 and Timer 0 of the PIC16F887 microcontroller. Timer 0 utilizes a 256 prescaler to measure time, while Timer 1 counts the rising edges of the input signal asynchronously.

To calculate the frequency, just divide the Timer 1 value by the elapsed time. However, to avoid using floating-point numbers, I employed an approximation method for division.

I hope the program proves useful to someone!! 


# Diagram connection

Signal == > RC0
This is the pin where you connect the signal to measure

Transistors == > RA1 to RA7
Base of the transistor to multiplex the 7 segments displays

Display 7 segments == > RB0 to RB7


