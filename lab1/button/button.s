# File: button.s
# ---------------
# LED connected to PB0, button connected to PC0
# Button configured with hardware pull-up resistor
# default state is high, when pressed, goes low
#

    lui     a0,0x2000       # a0 holds gpio base addr = 0x2000000
    addi    a1,zero,0x1     # a1 holds constant 1
    sw      a1,0x30(a0)     # config PB0 as output
    sw      a1,0x40(a0)     # turn on PB0
    sw      zero,0x60(a0)   # config PC0 as input

loop:
    lw      a2,0x70(a0) # record button input
    and     a2,a2,a1   # filter data
    beq     a2,zero,off #determine on or off

on: 
    sw      a1,0x40(a0) #tell LED to turn on
    j       loop #loop again

off:
    sw      zero,0x40(a0) #tell LED to turn off
    j       loop #loop again
