/* File: ps2_assign7.c
 * -------------------
 * ***** Obtains PS2 Scancode *****
 */
#include "gpio.h"
#include "gpio_extra.h"
#include "malloc.h"
#include "ps2.h"
#include "timer.h"
#include "gl.h"
#include "gpio_interrupt.h"
#include "interrupts.h"
#include "printf.h"
#include "ringbuffer.h"
#include "uart.h"

// A ps2_device is a structure that stores all of the state and information
// needed for a PS2 device. The clock field stores the gpio id for the
// clock pin, and the data field stores the gpio id for the data pin.
// Read ps2_new for example code that sets and uses these fields.
//
// You may extend the ps2_device structure with additional fields as needed.
// A pointer to the current ps2_device is passed into all ps2_ calls.
// Storing state in this structure is preferable to using global variables:
// it allows your driver to support multiple PS2 devices accessed concurrently
// (e.g., a keyboard and a mouse).
//
// This definition fills out the structure declared in ps2.h.

struct ps2_device {
    gpio_id_t clock;
    gpio_id_t data;
    unsigned long prevTime;
    unsigned int bytes;
    unsigned int count;
    rb_t *rb;
};

// Creates a new PS2 device connected to given clock and data pins,
// The gpios are configured as input and set to use internal pull-up
// (PS/2 protocol requires clock/data to be high default)

int errorCheckTime (ps2_device_t * dev){ //check if time between bits is too high, reset if it is
    if((timer_get_ticks() - (dev->prevTime))>(500*24)){ //greater than 500 us
        dev->count = 0;
        dev->bytes = 0;
        dev->prevTime = timer_get_ticks();
        return 0;
    }
    return 1;
}

void start(int data, ps2_device_t * dev){ //check if start bit is working
    if(data == 0){
        dev->count = 1;
        dev->bytes = 0;
        dev->prevTime = timer_get_ticks();
    }
} 

void addData(int data, ps2_device_t * dev){ //add data one by one
    if(errorCheckTime(dev)){
        dev->bytes = ((data)<<((dev->count)-1)) + dev->bytes;
        dev->count = dev->count + 1;
        dev->prevTime = timer_get_ticks();
    }
} 

void checkParity(int data, ps2_device_t * dev){ //check parity, or start over if it isn't odd
    if(errorCheckTime(dev)){
        dev->bytes = ((data)<<((dev->count)-1)) +  dev->bytes;
        int counter = 0;
        int input = dev->bytes;
        //count number of odd bits
        for(int i = 0; i < 9; i ++){
            if((input & 0b1)==1){
                counter++;
            }
            input = input >> 1;
        }
        //check if odd
        if((counter %2)!=1){
            dev->count = 0;
            dev->bytes = 0;
            dev->prevTime = timer_get_ticks();
        }
        else{
            dev->count = 10;
            dev->prevTime = timer_get_ticks();
        }
    }
} 

void checkStop(int data, ps2_device_t * dev){ //check the stop bit and reset if it is disunctional
    if(errorCheckTime(dev)){
        //check if stop bit is high
        if( data !=  1){
            dev->count = 0;
            dev->bytes = 0;
            dev->prevTime = timer_get_ticks();
        }
        else{
            if(!rb_full(dev->rb)){
                rb_enqueue(dev->rb,(dev->bytes & 0b11111111));
            }
            dev->count = 0;
            dev->bytes = 0;
            dev->prevTime = timer_get_ticks();
        }
    }
} 

void read_bit(void* auxdata){
    gpio_interrupt_clear(*(gpio_id_t *) auxdata);
    ps2_device_t * ps2Device =  ( ps2_device_t  *) (auxdata);
    int data = gpio_read(ps2Device->data);

    if( (ps2Device -> count) == 0){
        start(data,ps2Device);
    }
    else if( ((ps2Device -> count) >= 1) & ((ps2Device -> count) <= 8)){
        addData(data,ps2Device);
    }   
    else if( ((ps2Device -> count) == 9)){
        checkParity(data,ps2Device);
    }
    else{
        checkStop(data,ps2Device);
    }
}

ps2_device_t *ps2_new(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    // consider why must malloc be used to allocate device
    ps2_device_t *dev = malloc(sizeof(*dev));

    dev->clock = clock_gpio;
    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);

    dev -> bytes =0;
    dev -> count = 0;
    dev -> rb = rb_new();
    dev -> prevTime = timer_get_ticks();

    gpio_interrupt_init(); //interrupt enable
    gpio_interrupt_config(dev->clock, GPIO_INTERRUPT_NEGATIVE_EDGE, false);
    gpio_interrupt_register_handler(dev->clock, read_bit, dev);
    gpio_interrupt_enable(dev->clock);
    
    return dev;
}

uint8_t ps2_read(ps2_device_t *dev) {
    while(rb_empty(dev->rb));
    return ((uint8_t) rb_dequeue(dev->rb));
}

// uint8_t read_bit(ps2_device_t *dev){
//     int prev = 0;
//     while(1){
//         int c = gpio_read(dev->clock);
//         int d = gpio_read(dev->data);

//         if( (c) == 1){
//             prev = 1;
//         }

//         if( (c == 0) & prev){
//             return d;
//         }
//     }
// }

// //error correction for odd parity (0 for not working, 1 for working)
// int errorCheckOdd(unsigned int input){
//     int counter = 0;
//     //count number of odd bits
//     for(int i = 0; i < 9; i ++){
//         if((input & 0b1)==1){
//             counter++;
//         }
//         input = input >> 1;
//     }

//     //check if odd
//     if((counter %2)!=1){
//         return 0;
//     }

//     return 1;
// }

// //error correction for stop bit (0 not working, 1 for working)
// int errorCheckStop (unsigned int input){
//     //check if stop bit is high
//     input = input >> 9;
//     if( (input &0b1) != 1 ){
//         return 0;
//     }
//     return 1;
// }

// //error check scan code time
// int errorCheckTime (unsigned long prevTime){
//     if((timer_get_ticks() - prevTime)>(500*24)){ //greater than 500 us
//         return 0;
//     }
//     return 1;
// }

// // Read a single PS2 scancode. Always returns a correctly received scancode:
// // if an error occurs (e.g., start bit not detected, parity is wrong), the
// // function should read another scancode.
// uint8_t ps2_read(ps2_device_t *dev) {
//     unsigned int counter = 0;
//     unsigned int output = 0;

//     int readBit = 1;

//     while(readBit != 0){
//         readBit = read_bit(dev);
//     }

//     unsigned long prevTime = 0;
    
//     while (counter < 10){ //gather 10 bits
//         prevTime = timer_get_ticks();
//         output =  (read_bit(dev)<<counter) + output;
//         counter++;

//         if(!errorCheckTime(prevTime)){ //try again if failed check
//             return ps2_read(dev);
//         }

//         if(counter == 9){
//             if(!errorCheckOdd(output)){ //try again if failed check
//                 return ps2_read(dev);
//             }
//         }

//         if(counter == 10){
//             if(!errorCheckStop(output)){ //try again if failed check
//                 return ps2_read(dev);
//             }
//         }
//     }

//     //disscard last two bits;
//     uint8_t returnThing = (uint8_t) (output & 0b11111111);

//     return returnThing;
    
// }