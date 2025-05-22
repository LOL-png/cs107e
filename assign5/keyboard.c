/* File: keyboard.c
 * -----------------
 * ***** Turns scancode into a typable keyboard command *****
 */
#include "keyboard.h"
#include "ps2.h"

static ps2_device_t *dev;

//stores modifier state, 1 means active and 0 means off
static int modifierState[] = {0   ,0   ,0   ,0   };
                          //CAPS, CTRL, ALT,SHIFT

static int CAPScount = 0;

void keyboard_init(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    dev = ps2_new(clock_gpio, data_gpio);
}

uint8_t keyboard_read_scancode(void) {
    return ps2_read(dev);
}

key_action_t keyboard_read_sequence(void) { //read sequences
    uint8_t first = keyboard_read_scancode();
    if(first == 0xE0){ //modifier key
        first = keyboard_read_scancode();
        if(first == 0xF0){ //release
            first = keyboard_read_scancode();
            key_action_t action;
            action.what = 1;
            action.keycode = first;
            return action;
        }
        else{ //press
            key_action_t action;
            action.what = 0;
            action.keycode = first;
            return action;
        }
    }
    else if(first == 0xF0){ //release
        first = keyboard_read_scancode();
        key_action_t action;
        action.what = 1;
        action.keycode = first;
        return action;
    }
    else{ //press
        key_action_t action;
        action.what = 0;
        action.keycode = first;
        return action;
    }
}

key_event_t keyboard_read_event(void) {
    key_action_t action = keyboard_read_sequence();
    if( (action.keycode == 0x58)){//capslock
        if(action.what == 0){ 
            if(CAPScount==0){
                modifierState[0] =  modifierState[0]^1; //invert the state upon press
                CAPScount++;
            }
        }
        else{
            CAPScount = 0;
        }

        return keyboard_read_event();
    }

    else if( (action.keycode == 0x14)){//ctrl
        if(action.what == 0){ //set state to high on press
            modifierState[1] = 1;
        }
        else{ //set to low on release
            modifierState[1] = 0;
        }

        return keyboard_read_event();
    }

    else if(action.keycode == 0x11){//alt
        if(action.what == 0){ //set state to high on press
            modifierState[2] = 1;
        }
        else{ //set to low on release
            modifierState[2] = 0;
        }

        return keyboard_read_event();
    }

    else if( (action.keycode == 0x12) | (action.keycode == 0x59) ){//shift
        if(action.what == 0){ //set state to high on press
            modifierState[3] = 1;
        }
        else{ //set to low on release
            modifierState[3] = 0;
        }

        return keyboard_read_event();
    }

    key_event_t event;
    event.action = action;
    event.key = ps2_keys[action.keycode];

    keyboard_modifiers_t modifier; //set modifiers
    modifier = (modifierState[0] << 3) + (modifierState[1] << 2) + (modifierState[2]<<1) + modifierState[3];

    event.modifiers = modifier;

    return event;
}

char keyboard_read_next(void) {
    key_event_t keypress =  keyboard_read_event();
    if(keypress.action.what == 1){
        return keyboard_read_next(); //try again if it is the release key
    }
    
    if( ( ((keypress.modifiers >> 3)&0b1) == 0 ) & ( ((keypress.modifiers)&0b1 ) == 1) ){ //only shift on
        return keypress.key.other_ch; //other_ch
    }

    else if( ( ((keypress.modifiers >> 3)&0b1) == 1) & ( ((keypress.modifiers)&0b1 ) == 0) ){ //only caps locks
        if( (keypress.key.ch>='a') & (keypress.key.ch<='z')  ){
            return keypress.key.other_ch; //capitlize for only letters
        }
    }

    else if ( ( ((keypress.modifiers >> 3)&0b1) == 1) & ( ((keypress.modifiers)&0b1 ) == 1) ){ //both shift and caps locks, produce other_ch
        return keypress.key.other_ch;
    }

    return keypress.key.ch; 
    
}
