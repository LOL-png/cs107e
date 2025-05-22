/* File: test_keyboard_shell.c
 * ---------------------------
 * ***** Tests for keyboard and shell added *****
 */
#include "assert.h"
#include "keyboard.h"
#include "printf.h"
#include "shell.h"
#include "strings.h"
#include "uart.h"
#include "timer.h"

#define ESC_SCANCODE 0x76

static void test_keyboard_scancodes(void) {
    printf("\nNow reading single scancodes. Type ESC to finish this test.\n");
    while (1) {
        uint8_t scancode = keyboard_read_scancode();
        printf("[%02x]\n", scancode);
        if (scancode == ESC_SCANCODE) break;
    }
    printf("\nDone with scancode test.\n");
}

static void test_keyboard_sequences(void) {
    printf("\nNow reading scancode sequences (key actions). Type ESC to finish this test.\n");
    while (1) {
        key_action_t action = keyboard_read_sequence();
        printf("%s [%02x]\n", action.what == KEY_PRESS ? "  Press" :"Release", action.keycode);
        if (action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with scancode sequences test.\n");
}

static void test_keyboard_events(void) {
    printf("\nNow reading key events. Type ESC to finish this test.\n");
    while (1) {
        key_event_t evt = keyboard_read_event();
        printf("%s PS2_key: {%c,%c} Modifiers: 0x%x\n", evt.action.what == KEY_PRESS? "  Press" : "Release", evt.key.ch, evt.key.other_ch, evt.modifiers);
        if (evt.action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with key events test.\n");
}

static void test_keyboard_chars(void) {
    printf("\nNow reading chars. Type ESC to finish this test.\n");
    while (1) {
        char c = keyboard_read_next();
        if (c >= '\t' && c <= 0x80)
            printf("%c", c);
        else
            printf("[%02x]", c);
        if (c == ps2_keys[ESC_SCANCODE].ch) break;
    }
    printf("\nDone with key chars test.\n");
}

static void test_keyboard_assertMORE(void){
    char ch;


    //test every letter character and shift achievability
    for(int i = 0; i<26;i++){
        printf("\nType %c\n",i+0x61);
        ch = keyboard_read_next();
        assert(ch == (i+0x61));  // confirm user can follow directions and correct key char generated

        printf("\nHold Shift and Type %c\n",i+0x61);
        ch = keyboard_read_next();
        assert(ch == (i+0x41));  // confirm user can follow directions and correct key char generated
    }


    //various shift,caps, and other modifier scenarios

    printf("\nHold down Caps and type 'a'\n");
    ch = keyboard_read_next();
    assert(ch == 'A');  // confirm user can follow directions and correct key char generated

    printf("\nHold down Shift and Caps and type 'a'\n");
    ch = keyboard_read_next();
    assert(ch == 'A');  // confirm user can follow directions and correct key char generated

    printf("\nHold down Shift, Caps, and Alt  and type 'a'\n");
    ch = keyboard_read_next();
    assert(ch == 'A');  // confirm user can follow directions and correct key char generated

    printf("\nHold down Shift, Caps, Ctrl, and Alt and type 'a'\n");
    ch = keyboard_read_next();
    assert(ch == 'A');  // confirm user can follow directions and correct key char generated

    printf("\nHold down Shift and type '.'\n");
    ch = keyboard_read_next();
    assert(ch == '>');  // confirm user can follow directions and correct key char generated

    printf("\nHold down Shift and Caps and type '.'\n");
    ch = keyboard_read_next();
    assert(ch == '>');  // confirm user can follow directions and correct key char generated'

    printf("\nHold down Caps and type '.'\n"); //reset caps locks
    ch = keyboard_read_next();
    assert(ch == '.');  // confirm user can follow directions and correct key char generated'


}

static void test_keyboard_assert(void) {
    char ch;
    printf("\nHold down Shift and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'G');  // confirm user can follow directions and correct key char generated
}

static void test_shell_evaluate(void) {
    shell_init(keyboard_read_next, printf);

    printf("\nTest shell_evaluate on fixed commands.\n");
    int ret = shell_evaluate("echo hello, world!");
    printf("Command result is zero if successful, is it? %d\n", ret);
}

// This is an example of a "fake" input. When asked to "read"
// next character, returns char from a fixed string, advances index
static char read_fixed(void) {
    const char *input = "echo hello, world\nhelp\n";
    static int index;

    char next = input[index];
    index = (index + 1) % strlen(input);
    return next;
}

static void test_shell_readline_fixed_input(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(read_fixed, printf); // input is fixed sequence of characters

    printf("\nTest shell_readline, feed chars from fixed string as input.\n");
    printf("readline> ");
    shell_readline(buf, bufsize);
    printf("readline> ");
    shell_readline(buf, bufsize);
}

static void test_shell_readline_keyboard(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(keyboard_read_next, printf); // input from keybaord

    printf("\nTest shell_readline, type a line of input on ps2 keyboard.\n");
    printf("? ");
    shell_readline(buf, bufsize);
}

static char read_fixedMore(void) { //test for failed call of too short
    const char *input = "echo \b\b\b\b\b\b\b\b\b\b";
    static int index;

    char next = input[index];
    index = (index + 1) % strlen(input);
    return next;
}

static void test_shell_readline_fixed_inputMore(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(read_fixedMore, printf); // input is fixed sequence of characters

    printf("\nTest shell_readline, feed chars from fixed string as input.\n");
    printf("readline> ");
    shell_readline(buf, bufsize);
}

static char read_fixedMore2(void) { //test for failed call of too long
    const char *input = "echo ";
    static int index;

    char next = input[index];
    index = (index + 1) % strlen(input);
    return next;
}

static void test_shell_readline_fixed_inputMore2(void) {
    char buf[2];
    size_t bufsize = sizeof(buf);

    shell_init(read_fixedMore2, printf); // input is fixed sequence of characters

    printf("\nTest shell_readline, feed chars from fixed string as input.\n");
    printf("readline> ");
    shell_readline(buf, bufsize);
}

static char read_fixedMore3(void) { //test for failed call of too long
    const char *input = "poke "; //various string test
    static int index;

    char next = input[index];
    index = (index + 1) % strlen(input);
    return next;
}

static void test_shell_readline_fixed_inputMore3(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(read_fixedMore3, printf); // input is fixed sequence of characters

    printf("\nTest shell_readline, feed chars from fixed string as input.\n");
    printf("readline> ");
    shell_readline(buf, bufsize);
}

static void test_shell_evaluateMore(void){
    shell_init(keyboard_read_next, printf);

    printf("\nTest shell_evaluate on fixed commands.\n"); 
    int ret = shell_evaluate("peek 0x40000000"); //peek evaluations
    assert(ret==0);
    ret = shell_evaluate("peek 0x40000002");
    assert(ret != 0);
    ret = shell_evaluate("peek 45");
    assert(ret != 0);
    ret = shell_evaluate("peek Barbatos");
    assert(ret != 0);
    ret = shell_evaluate("peek 4056");
    assert(ret == 0);
    ret = shell_evaluate("peek 4056 what is this doing here");
    assert(ret == 0);

    ret = shell_evaluate("poke 4056 5000"); //poke evaluations
    assert(ret == 0);
    ret = shell_evaluate("poke Boy 5000");
    assert(ret != 0);
    ret = shell_evaluate("poke Boy Boy");
    assert(ret != 0);
    ret = shell_evaluate("poke 5000 Boy");
    assert(ret != 0);
    ret = shell_evaluate("poke 5000 Boy");
    assert(ret != 0);
    ret = shell_evaluate("poke 0x40 0x20");
    assert(ret == 0);
    ret = shell_evaluate("poke 0x40 4000");
    assert(ret == 0);
    ret = shell_evaluate("poke 0x40 4000 what is this doing here");
    assert(ret == 0);


    ret = shell_evaluate("help "); //help evaluations
    assert(ret == 0);
    ret = shell_evaluate("help please");
    assert(ret != 0);
    ret = shell_evaluate("poke Boy Boy");
    assert(ret != 0);
    ret = shell_evaluate("poke 5000 Boy");
    assert(ret != 0);
    ret = shell_evaluate("help clear");
    assert(ret == 0);
    ret = shell_evaluate("help clear what is this doing here");
    assert(ret == 0);
    ret = shell_evaluate("help help");
    assert(ret == 0);
    ret = shell_evaluate("help poke");
    assert(ret == 0);
    ret = shell_evaluate("help peek");
    assert(ret == 0);
    ret = shell_evaluate("help reboot");
    assert(ret == 0);

    printf("\nCompleted shell_evaluate\n"); 
}

void main(void) {
    uart_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    printf("Testing keyboard and shell.\n");

    test_keyboard_scancodes();
    timer_delay_ms(500);

    // test_keyboard_sequences();
    // timer_delay_ms(500);


    // test_keyboard_events();
    // timer_delay_ms(500);

    // test_keyboard_chars();

    // test_keyboard_assert();

    // test_keyboard_assertMORE();

    // test_shell_evaluate();

    // test_shell_readline_fixed_input();

    // test_shell_readline_fixed_inputMore();

    // test_shell_readline_fixed_inputMore2();

    test_shell_readline_fixed_inputMore3();

    // test_shell_readline_keyboard();

    // test_shell_evaluateMore();

    printf("Finished executing main() in test_keyboard_shell.c\n");
}
