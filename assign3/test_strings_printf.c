/* File: test_strings_printf.c
 * ---------------------------
 * ***** TODO: add your file header comment here *****
 */
#include "assert.h"
#include "printf.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

// Prototypes copied from printf.c to allow unit testing of helper functions
void num_to_string(unsigned long num, int base, char *outstr);
const char *hex_string(unsigned long val);
const char *decimal_string(long val);


static void test_memset(void) {
    char buf[25];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x7e, bufsize); // write 0x7e (char '~') to all mem locations
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 0x7e); // confirm value
}

static void test_memsetMore(void){
    char buf[25];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77e, bufsize); // write 0x7e (char '~') to all mem locations
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 0b01111110); // confirm LSByte function
}

static void test_strcmp(void) {
    assert(strcmp("apple", "apple") == 0);
    assert(strcmp("apple", "applesauce") < 0);
    assert(strcmp("pears", "apples") > 0);
}

static void test_strcmpMore(void) {
    assert(strcmp("applesauce", "apple") > 0); //confirm difference in size string
    assert(strcmp("b", "a") >  0); //confirm basic greater comparison
    assert(strcmp("a", "b") <  0); //confirm basic lesser comparison
    assert(strcmp("", "") ==0); //confirm empty string
    assert(strcmp("a", "") > 0); //confirm empty string
    assert(strcmp("\0", "") == 0); //confirm nullptr
    assert(strcmp("\0", "a") < 0);  //confirm nullptr
}

static void test_strlcat(void) {
    char buf[20];
    size_t bufsize = sizeof(buf);
    // as aid for debugging, initialize contents of buffer to repat value 0x7e
    // you can check contents later to confirm they have changed as expected
    memset(buf, 0x7e, bufsize);

    buf[0] = '\0'; // null at first index makes empty string
    assert(strlen(buf) == 0);
    strlcat(buf, "CS", bufsize); // append CS
    assert(strlen(buf) == 2);
    assert(strcmp(buf, "CS") == 0);
    strlcat(buf, "107e", bufsize); // append 107e
    assert(strlen(buf) == 6);
    assert(strcmp(buf, "CS107e") == 0);
}

static void test_strlcatMore(void) {
    char buf[4]; //lower buff size to test concat cutoff
    size_t bufsize = sizeof(buf);
    memset(buf, 0x7e, bufsize);

    buf[0] = '\0';
    assert(strlcat(buf, "CS1", bufsize)==3); // append CS107e (which should only do CS1)
    assert(strlen(buf) == 3);
    assert(strcmp(buf, "CS107e") < 0);

    char guf[4]; //test with all null characters
    size_t gufsize = sizeof(guf);
    memset(guf, 0x7e, gufsize);

    guf[0] = '\0';
    guf[1] = '\0';
    guf[2] = '\0';
    guf[3] = '\0';

    assert(strlcat(guf, "CS107e", gufsize)==6); // append CS107e (which should only do CS1)
    assert(strlen(guf) == 3);
    assert(strcmp(guf, "CS107e") < 0);


    char luf[] = "CS107E"; //buff size less than desintation space
    size_t lufsize = 4;

    assert(strlcat(luf, "CS", lufsize)==6); // append CS107e (which should only do CS1)
}


static void test_strtonum(void) {
    long val = strtonum("013", NULL);
    assert(val == 13);

    const char *input = "107rocks";
    const char *rest = NULL;

    val = strtonum(input, &rest);
    assert(val == 107);
    // rest was modified to point to first non-digit character
    assert(rest == &input[3]);
}

static void test_strtonumMore(void) {
    long val = strtonum("0x1A", NULL); //test hex
    assert(val == 0x1a);

    val = strtonum("0x1a", NULL); //test hex
    assert(val == 0x1a);

    const char *input = "0x1ag"; //test cutoff
    const char *rest = NULL;

    val = strtonum(input, &rest);
    assert(val == 0x1a);
    // rest was modified to point to first non-digit character
    assert(rest == &input[4]);
}

static void test_helpers(void) {
    char buf[32];
    size_t bufsize = sizeof(buf);
    // as aid for debugging, initialize contents of buffer to repat value 0x7e
    // you can check contents later to confirm they have changed as expected
    memset(buf, 0x7e, bufsize);

    num_to_string(209, 10, buf);
    assert(strcmp(buf, "209") == 0);
    num_to_string(209, 16, buf);
    assert(strcmp(buf, "d1") == 0);

    assert(strcmp(decimal_string(-99), "-99") == 0);
    assert(strcmp(hex_string(0x107e), "107e") == 0);
}

static void test_helpersMore(void) {
    char buf[32];
    size_t bufsize = sizeof(buf);
    // as aid for debugging, initialize contents of buffer to repat value 0x7e
    // you can check contents later to confirm they have changed as expected
    memset(buf, 0x7e, bufsize);

    num_to_string(0x8ab, 10, buf); //more compliated numbers
    assert(strcmp(buf, "2219") == 0);
    num_to_string(0x8ab, 16, buf);
    assert(strcmp(buf, "8ab") == 0);

    assert(strcmp(decimal_string(5882), "5882") == 0); //positive number
    assert(strcmp(hex_string(0xabcdef), "abcdef") == 0);
}

static void test_snprintf(void) {
    char luf[100];
    size_t lufsize = sizeof(luf);
    memset(luf, 0x7e, lufsize); // init array contents with fixed repeat value

    char *buf = luf;

    // // Try no formatting codes
    // assert(snprintf(buf, lufsize, "Hello, world!")==strlen("Hello, world!"));
    // assert(strcmp(buf, "Hello, world!") == 0);
    
    // String
    snprintf(buf, 5, "%ld", 999888777666555444L);
    uart_putstring(buf);
    assert(strcmp(buf, "9998") == 0);

    snprintf(buf, 10, "%x", 0x9abcdef0); 
    uart_putstring(buf);
    assert(strcmp(buf, "9abcdef0") == 0);

    // snprintf(buf, 20, "%x", 0xffffffff);
    // uart_putstring(buf);
    // assert(strcmp(buf, "ffffffff") == 0);

    // // dec
    // snprintf(buf, lufsize, "%d", 12);
    // assert(strcmp(buf, "12") == 0);
    
    // // hexi
    // snprintf(buf, lufsize, "%x",0xab);
    // assert(strcmp(buf, "ab") == 0);

    // // long dec
    // snprintf(buf, lufsize, "%ld", (long) 12);
    // assert(strcmp(buf, "12") == 0);
    
    // // long hexi
    // snprintf(buf, lufsize, "%lx",(long) 0xab);
    // assert(strcmp(buf, "ab") == 0);

    // snprintf(buf, lufsize, "%lx",(long) 0xabababab);
    // assert(strcmp(buf, "abababab") == 0);

    // // padding test
    // assert(snprintf(buf, lufsize, "%12s","blinky")==12);
    // assert(strcmp(buf, "      blinky") == 0);

    // assert(snprintf(buf, lufsize, "%4s","blinky")==6);
    // assert(strcmp(buf, "blinky") == 0);

    // assert(snprintf(buf, lufsize, "%12x",0xb13)==12);
    // assert(strcmp(buf, "000000000b13") == 0);

    // unsigned int* guf = (unsigned int *) 0x20000D0;

    // //padding pointer test
    // assert(snprintf(buf, lufsize, "%p",guf)==10);
    // assert(strcmp(buf, "0x020000d0") == 0);

    // //variable number of variables
    // assert(snprintf(buf, lufsize, "%12s%12s","blinky","blinky")==24);
    // assert(strcmp(buf, "      blinky      blinky") == 0);

    // // Test return value
    // assert(snprintf(buf, bufsize, "winky") == 5);
    // assert(snprintf(buf, 2, "winky") == 5);

    // From here it is up to you...!
}

static void test_snprintfMore(void){
    char luf[5];
    size_t lufsize = sizeof(luf);
    memset(luf, 0x7e, lufsize);

    // Try no formatting codes - with buffer cutoff
    assert(snprintf(luf, lufsize, "Hello, world!")==strlen("Hello, world!"));
    assert(strcmp(luf, "Hello") == 0);
}


static void test_printf(void){
    //basic printf test
    printf("I like %d dogs \n", 10);
}


// This function just here as code to disassemble for extension
int sum(int n) {
    int result = 6;
    for (int i = 0; i < n; i++) {
        result += i * 3;
    }
    return result + 729;
}

void test_disassemble(void) {
    const unsigned int add =  0x00f706b3;
    const unsigned int xori = 0x0015c593;
    const unsigned int bne =  0xfe061ce3;
    const unsigned int sd =   0x02113423;
    const unsigned int j =    0x0000006f;

    // formatting code %pI accesses the disassemble extension.
    // If extension not implemented, regular version of printf
    // will simply output pointer address followed by I
    // e.g.  "... disassembles to 0x07ffffd4I"
     printf("%pI\n",&add);
    printf("Encoded instruction %08x disassembles to %pI\n", add, &add);
    printf("Encoded instruction %08x disassembles to %pI\n", xori, &xori);
    printf("Encoded instruction %08x disassembles to %pI\n", bne, &bne);
    printf("Encoded instruction %08x disassembles to %pI\n", sd, &sd);
    printf("Encoded instruction %08x disassembles to %pI\n", j, &j);

    unsigned int *fn = (unsigned int *)sum; // disassemble instructions from sum function
    for (int i = 0; i < 10; i++) {
        printf("%p:  %08x  %pI\n", &fn[i], fn[i], &fn[i]);
    }
}

void main(void) {
    uart_init();
    uart_putstring("Start execute main() in test_strings_printf.c\n");

    test_memset();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    test_helpers();
    test_snprintf();
    test_disassemble(); // uncomment if you implement extension

    // TODO: Add more and better tests!
    test_memsetMore();
    test_strcmpMore();
    test_strlcatMore();
    test_strtonumMore();
    test_helpersMore();
    test_printf();

    uart_putstring("Successfully finished executing main() in test_strings_printf.c\n");
}
