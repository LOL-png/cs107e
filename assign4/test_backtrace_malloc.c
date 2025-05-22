/* File: test_backtrace_malloc.c
 * -----------------------------
 * Tests for backtrace and malloc functions
 */
#include "assert.h"
#include "backtrace.h"
#include "malloc.h"
#include "printf.h"
#include <stdint.h>
#include "strings.h"
#include "timer.h"
#include "uart.h"

void heap_dump(const char *label); // available in malloc.c but not public interface

static int recursion(int n) {
    printf("\nEnter call recursion(%d):\n", n);
    backtrace_print();
    if (n == 0) {
        return 0;
    } else if (n % 2 == 0) {  // even
        return 2 * recursion(n-1);
    } else {                   // odd
        return 1 + recursion(n-1);
    }
}

//test recursion using basic summation
static int summation(int n){
    printf("\nEnter call recursion(%d):\n", n);
    backtrace_print();
    if(n==1){
        return n;
    }
    else{
        return 1 + summation(n-1);
    }
}

//test simple function
static void simple(void){
    printf("\nSimple stack frame\n");
    backtrace_print();
    return;
}

static void show_frames(int nframes) {
    frame_t f[nframes];
    printf("\nEnter call show_frames(%d):\n", nframes);
    int frames_filled = backtrace_gather_frames(f, nframes);

    assert(frames_filled <= nframes);
    printf("Gathered backtrace contains %d frames:\n", frames_filled);
    backtrace_print_frames(f, frames_filled);
    printf("\n");
}

static void silly_goose(int nframes) {
    show_frames(nframes);
}

static void happy_day(void) {
    silly_goose(2);
    silly_goose(6);
    show_frames(3);
}

static void test_backtrace(void) {
    happy_day();
    recursion(4);
}

static void test_backtraceMore(void) {
    simple();
    summation(4);
}

static void good_egg(void *val) {
    // well-behaved function, stack canary will be intact at function end
    void *array[3];

    for (int i = 0; i < 3; i++) {
        array[i] = val;
    }
    printf("good_egg correctly used its stack array at addr %p\n", array);
}

static void bad_guy(int num_beyond, void *val) {
    // WARNING: buggy function writes beyond its stack buffer
    // stack canary will be overwritten, stack smash will be detected at function end
    void *array[3];

    for (int i = 0; i < 3 + num_beyond; i++) {
        array[i] = val;
    }
    printf("bad_guy wrote %d value(s) past end of stack array at addr %p\n", num_beyond, array);
}

void test_stack_protector(void) {
    // call to bad_guy function will write past end of stack buffer
    // If StackGuard is enabled, should halt and report stack smashing
    // If StackGuard not enabled, bad_guy will corrupt stack, consequence differs
    // based on what value overwrote the stack housekeeping data

    printf("\nTesting stack protector, be sure that stack-protector is enabled in Makefile.\n");
    void *val;
    // val = NULL;      // try different value to see change in consequence
    // val = (void *)0x40000010;
    // val = (void *)0x10000;
    // val = (void *)0x200000;
    val = (void *)0x600000;

    good_egg(val);
    for (int count = 1; count < 5; count++) {
        bad_guy(count, val);
    }
}

static void test_heap_dump(void) {
    heap_dump("Empty heap");

    int *p = malloc(sizeof(int));
    *p = 0;
    heap_dump("After p = malloc(4)");

    char *q = malloc(16);
    memcpy(q, "aaaaaaaaaaaaaaa", 16);
    heap_dump("After q = malloc(16)");

    free(p);
    heap_dump("After free(p)");

    free(q);
    heap_dump("After free(q)");
}

static void test_heap_dumpMore(void) {
    heap_dump("Empty heap");

    int *p = malloc(21);
    memcpy(p, "bbbb", 5); //continous size increase
    heap_dump("After p = malloc(\"bbbb\")");

    memcpy(p+1, "cccc", 5);
    heap_dump("After p = malloc(\"cccc\")");

    memcpy(p+2, "dddd", 5);
    heap_dump("After p = malloc(\"dddd\")");

    memcpy(p+3, "eeee", 5);
    heap_dump("After p = malloc(\"eeee\")");

    memcpy(p+4, "ffff", 5);
    heap_dump("After p = malloc(\"ffff\")");

    free(p);
    heap_dump("After free(p)");
}

static void test_heap_simple(void) {
    // allocate a string and array of ints
    // assign to values, check, then free
    const char *alphabet = "abcdefghijklmnopqrstuvwxyz";
    int len = strlen(alphabet);

    char *str = malloc(len + 1);
    memcpy(str, alphabet, len + 1);

    int n = 10;
    int *arr = malloc(n*sizeof(int));
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    heap_dump("abcdefghijklmnopqrstuvwxyz");

    assert(strcmp(str, alphabet) == 0);
    free(str);
    assert(arr[0] == 0 && arr[n-1] == n-1);
    free(arr);
}

static void test_heap_oddballs(void) {
    // test oddball cases
    char *ptr;

    ptr = malloc(900000000); // request too large to fit
    assert(ptr == NULL); // should return NULL if cannot service request
    heap_dump("After reject too-large request");

    ptr = malloc(0); // legal request, though weird
    heap_dump("After malloc(0)");
    free(ptr);

    free(NULL); // legal request, should do nothing
    heap_dump("After free(NULL)");
}

static void test_heap_multiple(void) {
    // array of dynamically-allocated strings, each
    // string filled with repeated char, e.g. "A" , "BB" , "CCC"
    // Examine each string, verify expected contents intact.

    int n = 8;
    char *arr[n];

    for (int i = 0; i < n; i++) {
        int num_repeats = i + 1;
        char *ptr = malloc(num_repeats + 1);
        memset(ptr, 'A' - 1 + num_repeats, num_repeats);
        ptr[num_repeats] = '\0';
        arr[i] = ptr;
    }
    heap_dump("After all allocations");
    for (int i = n-1; i >= 0; i--) {
        int len = strlen(arr[i]);
        char first = arr[i][0], last = arr[i][len -1];
        assert(first == 'A' - 1 + len);  // verify payload contents
        assert(first == last);
        free(arr[i]);
    }
    heap_dump("After all frees");
}

static void test_heap_More(void){
    int n = 0;
    while (n < 5){
        char *ptr = malloc(40); // request large space, reallocate, and request again for 5 times
        memcpy(ptr, "AAAA",5);
        assert(strcmp(ptr,"AAAA")==0);
        heap_dump("After request of AAAA");

        memcpy(ptr+4, "BBBB",5);
        assert(strcmp(ptr,"AAAABBBB")==0);
        heap_dump("After request of BBBB");

        char *ptrTwo = malloc(20);
        memcpy(ptrTwo, "CCCC",5);
        assert(strcmp(ptrTwo,"CCCC")==0);
        heap_dump("After request of CCCC");

        memcpy(ptrTwo+4, "DDDD",5);
        assert(strcmp(ptrTwo,"CCCCDDDD")==0);
        heap_dump("After request of DDDD");

        free(ptr);
        heap_dump("After request of free ptr");

        char * ptrThree = malloc(20);
        memcpy(ptrThree, "123456789ABCDEF",16); //test recycle for two block replacement
        assert(strcmp(ptrThree,"123456789ABCDEF")==0);
        assert(strcmp(ptrTwo,"CCCCDDDD")==0);
        heap_dump("After request of double block recycling");

        free(ptrTwo);
        assert(strcmp(ptrThree,"123456789ABCDEF")==0);
        heap_dump("After request of free ptrTwo");

        free(ptrThree);
        heap_dump("After request of free ptrThree");

        ptrThree = malloc(20);
        memcpy(ptrThree, "FEDCBA987654321",16);
        assert(strcmp(ptrThree,"FEDCBA987654321")==0);//test recycle two single block replacement
        heap_dump("After request of single block recycling ");
        free(ptrThree);
        heap_dump("After all free ");


        n++;
    }
}

static void test_recycle(void){
    for(int i = 0; i < 5; i++){
        char * ptr = malloc(10);
        memcpy(ptr,"AAAABBBBC",10);
        assert(strcmp(ptr,"AAAABBBBC")==0);

        heap_dump("After malloc AAAABBBBC ");

        free(ptr);

        heap_dump("After free AAAABBBBC ");

        char * ptrTwo = malloc (10);
        memcpy (ptrTwo,"123456789",10);
        assert(strcmp(ptrTwo,"123456789")==0);

        heap_dump("After malloc 123456789 ");

        free(ptrTwo);

        heap_dump("After free 123456789 ");
    }
}

static void test_split(void){
    char *ptr = malloc(50);
    memcpy(ptr,"0123456789012345678901234567890123456789012345678",50);
    assert(strcmp(ptr,"0123456789012345678901234567890123456789012345678")==0);
    heap_dump("After malloc 0123456789012345678901234567890123456789012345678 ");

    free(ptr);

    heap_dump("After free  0123456789012345678901234567890123456789012345678 ");

    char *ptr1 = malloc(5);
    memcpy(ptr1,"1234",5);
    assert(strcmp(ptr1,"1234")==0);

    char *ptr2 = malloc(5);
    memcpy(ptr2,"1234",5);
    assert(strcmp(ptr2,"1234")==0);

    char *ptr3 = malloc(5);
    memcpy(ptr3,"1234",5);
    assert(strcmp(ptr3,"1234")==0);

    char *ptr4 = malloc(5);
    memcpy(ptr4,"1234",5);
    assert(strcmp(ptr4,"1234")==0);

    char *ptr5 = malloc(5);
    memcpy(ptr5,"1234",5);
    assert(strcmp(ptr5,"1234")==0);

    char *ptr6 = malloc(5);
    memcpy(ptr6,"1234",5);
    assert(strcmp(ptr6,"1234")==0);

    char *ptr7 = malloc(5);
    memcpy(ptr7,"1234",5);
    assert(strcmp(ptr7,"1234")==0);

    char *ptr8 = malloc(5);
    memcpy(ptr8,"1234",5);
    assert(strcmp(ptr8,"1234")==0);

    char *ptr9 = malloc(5);
    memcpy(ptr9,"1234",5);
    assert(strcmp(ptr9,"1234")==0);

    char *ptr10 = malloc(5);
    memcpy(ptr10,"1234",5);
    assert(strcmp(ptr10,"1234")==0);

    heap_dump("After malloc 10x 1234 ");

    free(ptr1);
    free(ptr2);
    free(ptr3);
    free(ptr4);
    free(ptr5);
    free(ptr6);
    free(ptr7);
    free(ptr8);
    free(ptr9);
    free(ptr10);

    heap_dump("After all 10x 1234 free");

}

static void test_coalesce(void){
    char *ptr1 = malloc(5);
    memcpy(ptr1,"1234",5);
    assert(strcmp(ptr1,"1234")==0);

    char *ptr2 = malloc(5);
    memcpy(ptr2,"1234",5);
    assert(strcmp(ptr2,"1234")==0);

    char *ptr3 = malloc(5);
    memcpy(ptr3,"1234",5);
    assert(strcmp(ptr3,"1234")==0);

    char *ptr4 = malloc(5);
    memcpy(ptr4,"1234",5);
    assert(strcmp(ptr4,"1234")==0);

    char *ptr5 = malloc(5);
    memcpy(ptr5,"1234",5);
    assert(strcmp(ptr5,"1234")==0);

    char *ptr6 = malloc(5);
    memcpy(ptr6,"1234",5);
    assert(strcmp(ptr6,"1234")==0);

    char *ptr7 = malloc(5);
    memcpy(ptr7,"1234",5);
    assert(strcmp(ptr7,"1234")==0);

    char *ptr8 = malloc(5);
    memcpy(ptr8,"1234",5);
    assert(strcmp(ptr8,"1234")==0);

    char *ptr9 = malloc(5);
    memcpy(ptr9,"1234",5);
    assert(strcmp(ptr9,"1234")==0);

    char *ptr10 = malloc(5);
    memcpy(ptr10,"1234",5);
    assert(strcmp(ptr10,"1234")==0);

    heap_dump("After malloc 10x 1234 ");

    free(ptr10);
    free(ptr9);
    free(ptr8);
    free(ptr7);
    free(ptr6);
    free(ptr5);
    free(ptr4);
    free(ptr3);
    free(ptr2);
    free(ptr1);

    heap_dump("After reverse order free of 10x 1234");

    char *ptr = malloc(50);
    memcpy(ptr,"0123456789012345678901234567890123456789012345678",50);
    assert(strcmp(ptr,"0123456789012345678901234567890123456789012345678")==0);
    heap_dump("After malloc 0123456789012345678901234567890123456789012345678 ");

    free(ptr);

    heap_dump("After free  0123456789012345678901234567890123456789012345678 ");

}

static void test_heap_leaks(void) {
    // This function allocates blocks which are never freed.
    // Leaks will be reported if doing the Valgrind extension, but otherwise
    // they are harmless/silent
    char *ptr;

    ptr = malloc(9); // leaked
    ptr = malloc(5);
    free(ptr);
    ptr = malloc(107); // leaked
}

void test_heap_redzones(void) {
    // DO NOT ATTEMPT THIS TEST unless your heap has red zone protection!
    char *ptr;

    ptr = malloc(9);
    memset(ptr, 'a', 9); // write into payload
    free(ptr); // ptr is OK

    ptr = malloc(5);
    ptr[-1] = 0x45; // write before payload
    free(ptr);      // ptr is NOT ok

    ptr = malloc(12);
    ptr[13] = 0x45; // write after payload
    free(ptr);      // ptr is NOT ok
}

void main(void) {
    uart_init();
    uart_putstring("Start execute main() in test_backtrace_malloc.c\n");

    // test_backtrace();
    // test_backtraceMore();
    test_stack_protector(); // Selectively uncomment when ready to test this

    // test_heap_dump();
    // test_heap_dumpMore();
    // test_heap_simple();
    // test_heap_oddballs();
    // test_heap_multiple();

    // test_heap_More();
    // test_recycle();
    // test_split();
    // test_coalesce();
    // test_heap_leaks();
    // test_heap_redzones(); // DO NOT USE unless you have implemented red zone protection!
    uart_putstring("\nSuccessfully finished executing main() in test_backtrace_malloc.c\n");
}
