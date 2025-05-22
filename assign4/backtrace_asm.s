/* File: backtrace_asm.s
 * ---------------------
 * ***** This function returns the current stack frame
 */

.globl backtrace_get_fp
backtrace_get_fp:
    add a0, s0, zero
    ret
