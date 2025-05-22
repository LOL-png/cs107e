#include "printf.h"
#include "timer.h"
#include "uart.h"

void main (void) {
    volatile unsigned int *pb_config0 = (unsigned int *)0x02000030;

    uart_init();    // must set up uart peripheral before using, init once


    for (int i = 0; i < 5; i++) {
        uart_putstring("hello, laptop\n");
        timer_delay(1);
        printf("Loop  %d \n", i);
    }

    printf("pb config orig: %x \n",*pb_config0);

    gpio_set_output(GPIO_PB3);
    gpio_set_output(GPIO_PB7);

    printf("pb config output: %x \n",*pb_config0);

    gpio_set_input(GPIO_PB3);
    gpio_set_input(GPIO_PB7);
 
    printf("pb config input: %x \n",*pb_config0);

    printf("We %s printf!\n", "<3");
} 
