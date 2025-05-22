/* File: gpio.c
 * ------------
 * GPIO pin setup based upon gpio.h
 */
#include "gpio.h"
#include "timer.h"
#include "gpio_extra.h"
#include <stddef.h>

enum { GROUP_B = 0, GROUP_C, GROUP_D, GROUP_E, GROUP_F, GROUP_G };

typedef struct  {
    unsigned int group;
    unsigned int pin_index;
} gpio_pin_t;

// The gpio_id_t enumeration assigns a symbolic constant for each
// in such a way to use a single hex constant. The more significant
// hex digit identifies the group and lower 2 hex digits are pin index:
//       constant 0xNnn  N = which group,  nn = pin index within group
//
// This helper function extracts the group and pin index from a gpio_id_t
// e.g. GPIO_PB4 belongs to GROUP_B and has pin_index 4
static gpio_pin_t get_group_and_index(gpio_id_t gpio) {
    gpio_pin_t gp;
    gp.group = gpio >> 8;
    gp.pin_index = gpio & 0xff; // lower 2 hex digits
    return gp;
}

// The gpio groups are differently sized, e.g. B has 13 pins, C only 8.
// This helper function confirms that a gpio_id_t is valid (group
// and pin index are valid)
bool gpio_id_is_valid(gpio_id_t pin) {
    gpio_pin_t gp = get_group_and_index(pin);
    switch (gp.group) {
        case GROUP_B: return (gp.pin_index <= GPIO_PB_LAST_INDEX);
        case GROUP_C: return (gp.pin_index <= GPIO_PC_LAST_INDEX);
        case GROUP_D: return (gp.pin_index <= GPIO_PD_LAST_INDEX);
        case GROUP_E: return (gp.pin_index <= GPIO_PE_LAST_INDEX);
        case GROUP_F: return (gp.pin_index <= GPIO_PF_LAST_INDEX);
        case GROUP_G: return (gp.pin_index <= GPIO_PG_LAST_INDEX);
        default:      return false;
    }
}

// This helper function is suggested to return the address of
// the config0 register for a gpio group, i.e. get_cfg0_reg(GROUP_B)
// Refer to the D1 user manual to learn the address the config0 register
// for each group. Be sure to note how the address of the config1 and
// config2 register can be computed as relative offset from config0.
// (okay to discard this function if it doesn't fit with your design)
static volatile unsigned int *get_cfg0_reg(unsigned int group) {
    if(group == GROUP_B){
        return( unsigned int *)  (0x2000030);
    }

    else if (group == GROUP_C){
        return (unsigned int *)  (0x2000060);
    }

    else if (group == GROUP_D){
        return (unsigned int *) (0x2000090);
    }

    else if (group == GROUP_E){
        return (unsigned int *)  (0x20000C0);
    }

    else if (group == GROUP_F){
        return (unsigned int *)  (0x20000F0);
    }

    else {
        return (unsigned int *)  (0x2000120);
    }
}

// This helper function is suggested to return the address of
// the data register for a gpio group. Refer to the D1 user manual
// to learn the address of the data register for each group.
// (okay to discard this function if it doesn't fit with your design)
static volatile unsigned int *get_data_reg(unsigned int group) {
    if(group == GROUP_B){
        return (unsigned int *) (0x2000040);
    }

    else if (group == GROUP_C){
        return (unsigned int *) (0x2000070);
    }

    else if (group == GROUP_D){
        return (unsigned int *) (0x20000A0);
    }

    else if (group == GROUP_E){
        return (unsigned int *)  (0x20000D0);
    }

    else if (group == GROUP_F){
        return (unsigned int *)  (0x2000100);
    }

    else {
        return (unsigned int *)  (0x2000130);
    }
}

//pullup registers
static volatile unsigned int *get_pullup_reg(unsigned int group) {
    if(group == GROUP_B){
        return( unsigned int *)  (0x2000054);
    }

    else if (group == GROUP_C){
        return (unsigned int *)  (0x2000084);
    }

    else if (group == GROUP_D){
        return (unsigned int *)  (0x20000B4);
    }

    else if (group == GROUP_E){
        return (unsigned int *)  (0x20000E4);
    }

    else if (group == GROUP_F){
        return (unsigned int *)  (0x2000114);
    }

    else {
        return (unsigned int *)  (0x2000144);
    }
}

void gpio_init(void) {
    // no initialization required for this peripheral
}

void gpio_set_input(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_INPUT);
}

void gpio_set_output(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_OUTPUT);
}

//gpio_set_function takes in a desired pin and sets that pin to be either input or output
void gpio_set_function(gpio_id_t pin, unsigned int function) {

    //check if pin is valid and function is less than 16; if not, do nothing
    if((!gpio_id_is_valid(pin)) | (function >= 16)){
        return;
    }

    //assign index and memory address
    gpio_pin_t outputReg = get_group_and_index(pin);

    volatile unsigned int *PB =  (unsigned int *) get_cfg0_reg(outputReg.group);  
    unsigned int index = 0;

    //converts the pin to correct memory address; 
    //also converts pin number to a bit index
    if(outputReg.pin_index >= 16){
        PB += 2;
        index = outputReg.pin_index - 16;
    }
    
    else if(outputReg.pin_index >= 8 ){
        PB += 1;
        index = outputReg.pin_index - 8;
    }

    else{
        index = outputReg.pin_index;
    }
    
    unsigned int constantOne = 0b1111;
    unsigned int temp = 0;

    //reads every four bits and either adds it to temp, or adds the function
    for(int i = 7; i >= 0 ; i -- ){
        temp = temp << 4;

        if(i == index){
            temp = temp + function;
        }

        else{
            temp = temp + ( (*PB >> i*4) & constantOne);
        }

    }

    //set PB to temp
    *PB = temp;

}

//gpio_get_function takes in a pin and returns the function a pin has been set to
unsigned int gpio_get_function(gpio_id_t pin) {

    //check if pin is valid; if not, return GPIO_INVALID_REQUEST
    if(!gpio_id_is_valid(pin)){
        return GPIO_INVALID_REQUEST;
    }
    
    //assign index and memory address
    gpio_pin_t outputReg = get_group_and_index(pin);

    volatile unsigned int *PB =  (unsigned int *) get_cfg0_reg(outputReg.group);  
    unsigned int index = 0;

    //converts the pin to correct memory address; 
    // also converts pin number to a bit index
    if(outputReg.pin_index >=  16){
        PB += 2;
        index = outputReg.pin_index - 16;
    }
    
    else if(outputReg.pin_index >= 8 ){
        PB += 1;
        index = outputReg.pin_index - 8;
    }

    else{
        index = outputReg.pin_index;
    }

    unsigned int constantOne = 0b1111;

    //returns the four bits starting from the specific index
    return  (*PB >> index*4) & constantOne;
}

//gpio_write writes a specified pin to high or low (1 or 0)
void gpio_write(gpio_id_t pin, int value) {
    //check if pin is valid; if not, do nothing
    if(!gpio_id_is_valid(pin)){
        return;
    }

    //reconfigure value
    if(value !=0){
        value = 1;
    }

    //assign index and memory address
    gpio_pin_t outputReg = get_group_and_index(pin);

    volatile unsigned int *PB =  (unsigned int *) get_data_reg(outputReg.group);  
    unsigned int index = outputReg.pin_index;

    //creates a constant stream of 1's with length (index-1)
    unsigned int constantOne = 0b0;

    for(int i = 0; i < index;i++){
        constantOne = constantOne << 1;
        constantOne += 0b1;
    }

    //temporary stores the bits to the right of index
    unsigned int store = *PB & constantOne;

    //deletes bits including index and to the right
    unsigned int output = *PB >> (index+1);

    //assign new bit to index and add back the stored bits
    output = output << 1;
    output += value;
    output = output << index;
    output += store;

    //reassign PB
    *PB = output;
}

//gpio_read reads if a given pin is high or low
int gpio_read(gpio_id_t pin) {
    //check if pin is valid; if not, return GPIO_INVALID_REQUEST
    if(!gpio_id_is_valid(pin)){
        return GPIO_INVALID_REQUEST;
    }

    //assign index and memory address
    gpio_pin_t outputReg = get_group_and_index(pin);

    volatile unsigned int *PB =  (unsigned int *) get_data_reg(outputReg.group);  
    unsigned int index = outputReg.pin_index;

    //creates a constant of 1
    unsigned int constantOne = 0b1;

    //deletes bits to the right of index
    unsigned int output = *PB >> (index);

    return output & constantOne;
}

//set pullup resistor
void gpio_set_pullup(gpio_id_t pin) {
    //check if pin is valid and function is less than 16; if not, do nothing
    if((!gpio_id_is_valid(pin))){
        return;
    }

    //assign index and memory address
    gpio_pin_t outputReg = get_group_and_index(pin);

    volatile unsigned int *PB =  (unsigned int *) get_pullup_reg(outputReg.group);  
    unsigned int index = 0;

    //converts the pin to correct memory address; 
    //also converts pin number to a bit index
    if(outputReg.pin_index >= 16){
        PB += 1;
        index = outputReg.pin_index - 16;
    }

    else{
       index =  outputReg.pin_index;
    }
    
    unsigned int constantOne = 0b11;
    unsigned int temp = 0;

    //reads every two bits and either adds it to temp, or adds the function
    for(int i = 15; i >= 0 ; i -- ){
        temp = temp << 2;

        if(i == index){
            temp = temp + 0b01;
        }

        else{
            temp = temp + ( (*PB >> i*2) & constantOne);
        }

    }

    //set PB to temp
    *PB = temp;
}

