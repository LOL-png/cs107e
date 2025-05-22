/* File: extension.s
 * --------------
 * *****Code created by Howard Ji to create Larson Scanner 2.0, an updated version with a brightness gradient *****
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
    
    add     a4,zero,zero    # a4 reconfigured to count "main LED on" for scanner
    add     a5,zero,a1      # a5 tracks forward/backward scan
    addi    a6,zero,7       # a6 holds constant 7

 loop:
    lui     s1,250          # s1 = init countdown value
 
    addi    a3,s1,-4        # a3 is countdown for 1st brightness
    addi    a7,s1,-3        # a7 is countdown for 2nd brightness
    addi    a2,s1,-2        # a2 is countdown for 3rd brightness

 brightnessSetup:
    add     s2,zero,a1      # s2 stores if 1st brightness LED should be on
    add     s3,zero,a1      # s3 stores if 2nd brightness LED should be on
    add     s4,zero,a1      # s4 stores if 3rd brightness LED should be on

 brightnessThree:
    bne    s1,a2,brightnessTwo # skips to brightnessTwo 
    xori   s4,s4,1             # switches the state s4, if the 3rd brightness LED should be on
    addi   a2,a2,-2            # decrements a2
    
 brightnessTwo:
    bne    s1,a7,brightnessOne    # skips to brightnessOne
    xori   s3,s3,1                # switches the state s3, if the 2nd brigthness LED should be one
    addi   a7,a7,-3               # decrements a7

 brightnessOne:
    bne    s1,a3,construct     # skips to construct
    xori   s2,s2,1             # switches the state s2, if the 1st brightness LED should be on
    addi   a3,a3,-4            # decrements a3

 construct:
    add    s5,zero,zero        # s5 builds which pins are on
    addi   s9,zero,8           # s9 is a constant 8

 constructThreeRight:
    addi   s8,a4,3                    # s8 is s4 + 3 to represent the position of the 3rd bright right LED
    bge    s8,s9,constructTwoRight    # s8 is checked if the position is possible on the board

    addi   s6,a4,3                    # configure s6 to be shift increment to place LED state
    sll    s7,s4,s6                   # configure s7 to hold the current LED state
    add    s5,s5,s7                   # add the curent LED state with other LED states
 
 constructTwoRight:
    addi   s8,a4,2                    # s8 is s4 + 2 to represent the position of the 2nd bright right LED
    bge    s8,s9,constructOneRight    # s8 is checked if the position is possible on the board

    addi   s6,a4,2                    # configure s6 to be shift increment to place LED state
    sll    s7,s3,s6                   # configure s7 to hold the current LED state
    add    s5,s5,s7                   # add the curent LED state with other LED states

 constructOneRight:
    addi   s8,a4,1                    # s8 is s4 + 1 to represent the position of the 1st bright right LED
    bge    s8,s9,constructA4          # s8 is checked if the position is possible

    addi   s6,a4,1                    # configure s6 to be shift increment to place LED state
    sll    s7,s2,s6                   # configure s7 to hold the current LED state
    add    s5,s5,s7                   # add the curent LED state with other LED states

 constructA4:
    sll    s7,a1,a4                   # configure s7 to hold the current LED state
    add    s5,s5,s7                   # add a4 LED state with other LED states

 constructOneLeft:
    addi   s8,a4,-1                   # s8 is s4 - 1 to represent the position of the 1st bright left LED
    blt    s8,zero,constructTwoLeft   # s8 is checked if the position is possible

    addi   s6,a4,-1                   # configure s6 to be shift increment to place LED state
    sll    s7,s2,s6                   # configure s7 to hold the current LED state
    add    s5,s5,s7                   # add the curent LED state with other LED states

 constructTwoLeft:
    addi   s8,a4,-2                   # s8 is s4 - 2 to represent the position of the 2nd bright left LED
    blt    s8,zero,constructThreeLeft # s8 is checked if the position is possible

    addi   s6,a4,-2                   # configure s6 to be shift increment to place LED state
    sll    s7,s3,s6                   # add the curent LED state with other LED states
    add    s5,s5,s7                   # add the curent LED state with other LED states

 constructThreeLeft:
    addi   s8,a4,-3                   # s8 is s4 - 3 to represent the position of the 3rd bright left LED
    blt    s8,zero,build              # s8 is checked if the position is possible

    addi   s6,a4,-3                   # configure s6 to be shift increment to place LED state
    sll    s7,s4,s6                   # add the curent LED state with other LED states
    add    s5,s5,s7                   # add the curent LED state with other LED states
    
 build:   
    sw      s5,0x40(a0)           # set data value of PB pin to s5

 delay:
    addi    s1,s1,-1              # decrement s1
    beq     s1,zero,forwardScan   # when s1 is 0, send to forwardScan  
    j       brightnessSetup       # repeat the loop

 forwardScan:
    beq     a5,zero,backwardScan # skips to backward scan
    beq     a4,a6,switch         # checks if it is time to switch scan direction
    addi    a4,a4,1              # increments a4 to drive next pin on

 backwardScan:
    beq     a5,a1,continue       # skips to continue
    beq     a4,zero,switch       # checks if it is time to switch scan direction
    addi    a4,a4,-1             # decrements a4 to drive next pin on
     
 continue:
    j       loop            # back to top of outer loop

 switch:
    xori a5,a5,1            # switches scanning direction
    j       forwardScan     # returns back to forwardScan
