/* File: clock.c
 * -------------
 * ***** This code creates and displays a clock
 */
#include "gpio.h"
#include "timer.h"

/* __Note__: Do not edit/remove the three lines of code below.
 * These preprocessor directives cooperate with Makefile to allow
 * setting DURATION as part of the build process. You can test your
 * clock application on the duration of your choice by specifying
 * value when invoking make like so
 *       make run DURATION=30
 * The value for DURATION is expressed in seconds and defaults to
 * 67 seconds if not explicitly set. (i.e. 1 min 7 seconds)
 */
#ifndef DURATION
#define DURATION 67
#endif


//stores numbers 0 to 9 with a binary representation
const static unsigned char NUMBERCHART[] = 
    {0b00111111,
     0b00000110,
     0b01011011,
     0b01001111,
     0b01100110,
     0b01101101,
     0b01111101,
     0b00000111,
     0b01111111,
     0b01100111,
    };

//permanent pins
const static int N_SEG = 7, N_DIG = 4;
const static gpio_id_t SEGMENT[] = {GPIO_PD17, GPIO_PB6, GPIO_PB12, GPIO_PB11, GPIO_PB10, GPIO_PD11, GPIO_PD13};
const static gpio_id_t DIGIT[] = {GPIO_PB4, GPIO_PB3, GPIO_PB2, GPIO_PC0};
const static gpio_id_t BUTTON = GPIO_PD12;

//helper implements turning on a single sequence of digits for one second
void countdownClockHelper(int input[]){
    //loop infinitely displaying all four digits for a second
    int count = 100;
    while(count > 0){
        for(int i = 0; i < 4;i++){
            //turn on specified digit
            gpio_write(DIGIT[i],1);

            unsigned char number = NUMBERCHART[input[i]];

            //loop through each segment LED and turn on based on number
            for(int i = 0; i < N_SEG;i++){
                unsigned char letterSeg = number & 0b1;
                if(letterSeg == 1){
                    gpio_write(SEGMENT[i],1);
                }
                else{
                    gpio_write(SEGMENT[i],0);
                }

                number = number >> 1;
            }

            timer_delay_us(2500); //delay
    
            //turn off specified digit
            gpio_write(DIGIT[i],0);

            //loop through all segments and turn all off
            for(int i = 0; i < N_SEG;i++){
                gpio_write(SEGMENT[i],0);
            }
        }
        count --;

    }
}

//ending message blinker
void blinkMessage(void){
    //set delay first
    timer_delay_ms(1000);

    //turn on and off pattern
    int counter = 100;
    while(counter > 0){
        for(int i = 0; i < 4;i++){
            //turn on specified digit
            gpio_write(DIGIT[i],1);

            unsigned char number = NUMBERCHART[0];

            //loop through each segment LED and turn on based on number
            for(int i = 0; i < N_SEG;i++){
                unsigned char letterSeg = number & 0b1;
                if(letterSeg == 1){
                    gpio_write(SEGMENT[i],1);
                }
                else{
                    gpio_write(SEGMENT[i],0);
                }

                number = number >> 1;
            }

            timer_delay_us(2500); //delay
    
            //turn off specified digit
            gpio_write(DIGIT[i],0);

            //loop through all segments and turn all off
            for(int i = 0; i < N_SEG;i++){
                gpio_write(SEGMENT[i],0);
            }
        }

        //set delay for off time
        counter--;
        if(counter <= 0){
            counter = 100;
            timer_delay_ms(1000);
        }
    }
}

//implements a countdown and displays it through the clock
void countdownClock (int countdown){

    //don't run if countdown is greater than maximum possible
    if( (countdown > 5999) | (countdown < 0)){
        return;
    }

    //retrieve minutes, seconds, and total time
    int minutes = countdown/60;
    int seconds = countdown - minutes*60;

    //mark each segment for output
    for (int i = 0; i < N_SEG; i++) {   
        gpio_set_output(SEGMENT[i]);
    }

    //mark each digit for output
    for (int i = 0; i < N_DIG; i++) {
        gpio_set_output(DIGIT[i]);
    }
   
    //configure button as input
    gpio_set_input(BUTTON); 

    //pause until button is pressed
    while (gpio_read(BUTTON) == 1); 

    //countdown and loop through values
    while(true){

        //create four digits as inputs to helper
        int first = minutes/10;
        int second = minutes-first*10;
        int third = seconds/10;
        int fourth = seconds - third*10;
        int inputArray[] = {first,second,third,fourth};

        countdownClockHelper(inputArray);

        if(seconds == 0){
            if (minutes == 0){
                //send ending blink message
                blinkMessage();
            }
            else{
                minutes--;
                seconds = 59;
            }
        }
        else{
            seconds--;
        }
    }  
}

int main(void) {
    int countdown = DURATION;

    gpio_init();
    timer_init();

    //test countdownClock
    countdownClock(countdown);

    return countdown;
}
