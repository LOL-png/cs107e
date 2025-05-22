/* File: backtrace.c
 * -----------------
 * ***** Implementation of backtrace *****
 */
#include "backtrace.h"
#include "mango.h"
#include "printf.h"
#include "symtab.h"
#include "strings.h"

// helper function implemented in file backtrace_asm.s
extern unsigned long backtrace_get_fp(void);

//gathers all frames into frame_t
int backtrace_gather_frames(frame_t f[], int max_frames) {
    unsigned long * fp = (unsigned long *) (backtrace_get_fp()-16); //grab fp
    int n = 0;
    while(( ((unsigned long) fp) <= 0x4ffffff0) & (n < max_frames)){
        f[n].resume_addr = (uintptr_t) *(fp+sizeof(char)); //the current memory address pointed to is the current fp
        fp = (unsigned long *) (*fp-16); //fp's data is the previous memory address (set with some offset)
        n++;
    }
    return n;
}

void backtrace_print_frames(frame_t f[], int n) {
    char labelbuf[128];

    for (int i = 0; i < n; i++) {
        symtab_label_for_addr(labelbuf, sizeof(labelbuf), f[i].resume_addr);
        printf("#%d 0x%08lx at %s\n", i, f[i].resume_addr, labelbuf);
    }
}

void backtrace_print(void) {
    int max = 50;
    frame_t arr[max];

    int n = backtrace_gather_frames(arr, max);
    backtrace_print_frames(arr+1, n-1);   // print frames starting at this function's caller
}


long __stack_chk_guard = 0x7e; //tilda character

void __stack_chk_fail(void)  {
    unsigned long * fp = (unsigned long *) (backtrace_get_fp()); //first frame address is stored in memory
    unsigned long ra =   ( unsigned long) *(fp-1); //ra value
    ra-=8;
    char labelbuf[128];
    symtab_label_for_addr(labelbuf,sizeof(labelbuf),ra);
    char labelbufTwo[128];
    int index = 0;
    for(; index < 128;index++){
        if(labelbuf[index]=='+'){
            break;
        }
    }
    memcpy(labelbufTwo,(labelbuf+1),index-1);
    labelbufTwo[index-1] = '\0';
    printf("\n*** Stack smashing detected at end of function %s() ***",labelbufTwo);
    while (1){
        mango_abort();
    } // noreturn function must not return control to caller
}
