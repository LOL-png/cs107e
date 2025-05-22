/* File: timer_asm.s
 * ------------------
 * ***** Returns the current tick
 */

.attribute arch, "rv64imac_zicsr"

.globl timer_get_ticks
timer_get_ticks:
    csrr a0, time #load memory address of clock
    ret
