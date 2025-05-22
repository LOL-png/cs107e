/* File: larson.s
 * --------------
 * *****Code created by Howard Ji to create Larson Scanner *****
 */

    lui     a0,0x2000       # a0 holds base addr PB group = 0x2000000
    addi    a1,zero,1       # a1 holds constant 1
    addi    a3,zero,1       # a3 holds PB0 to PB7 output pins
    addi    a4,zero,7       # a4 is countdown value

 pinSetup:
    slli    a3,a3,4          # shifts a3 by 4 left 
    addi    a3,a3,1          # adds a 1 for each PB to be configured to output
    addi    a4,a4,-1         # decrements a4
    bne     a4,zero,pinSetup # loops again for all PB pins 
    
    sw      a3,0x30(a0)     # configure PB0 to PB7 as output
    
    add     a4,zero,zero    # a4 reconfigured to count for scanner
    add     a5,zero,a1      # a5 tracks forward/backward scan
    addi    a6,zero,7       # a6 holds constant 7

 loop:          
    sll     a1,a1,a4        # shift a1 right by a4 
    sw      a1,0x40(a0)     # set data value of PB pin to a1
    srl     a1,a1,a4        # shift a1 left by a4      

 forwardScan:
    beq     a5,zero,backwardScan # skips to backward scan
    beq     a4,a6,switch         # checks if it is time to switch scan direction
    addi    a4,a4,1              # increments a4 to drive next pin on

 backwardScan:
    beq     a5,a1,continue       # skips to delay
    beq     a4,zero,switch       # checks if it is time to switch scan direction
    addi    a4,a4,-1             # decrements a4 to drive next pin on
     
 continue:
    lui     a2,4000        # a2 = init countdown value

 delay:
    addi    a2,a2,-1        # decrement a2
    bne     a2,zero,delay   # keep counting down until a2 is zero

    j       loop            # back to top of outer loop
 
 switch:
    xori a5,a5,1            # switches scanning direction
    j       forwardScan     # returns back to forwardScan
