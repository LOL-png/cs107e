/* File: test_gl_console.c
 * -----------------------
 * ***** Tests for fb, gl, and console *****
 */
#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"

static void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in tio to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}


static void test_fb(void) {
    const int SIZE = 500;
    fb_init(SIZE, SIZE, FB_SINGLEBUFFER); // init single buffer

    assert(fb_get_width() == SIZE);
    assert(fb_get_height() == SIZE);
    assert(fb_get_depth() == 4);

    void *cptr = fb_get_draw_buffer();
    assert(cptr != NULL);
    int nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0x99, nbytes); // fill entire framebuffer with light gray pixels
    pause("Now displaying 500 x 500 screen of light gray pixels");

    fb_init(1280, 720, FB_DOUBLEBUFFER); // init double buffer
    cptr = fb_get_draw_buffer();
    nbytes =  fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0xff, nbytes); // fill one buffer with white pixels
    fb_swap_buffer();
    pause("Now displaying 1280 x 720 white pixels");

    cptr = fb_get_draw_buffer();
    memset(cptr, 0x33, nbytes); // fill other buffer with dark gray pixels
    fb_swap_buffer();
    pause("Now displaying 1280 x 720 dark gray pixels");

    for (int i = 0; i < 5; i++) {
        fb_swap_buffer();
        timer_delay_ms(250);
    }
}

static void test_gl(void) {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    // Double buffer mode, make sure you test single buffer too!
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    assert(gl_get_height() == HEIGHT);
    assert(gl_get_width() == WIDTH);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55)); // create purple color

    // Draw green pixel in lower right
    gl_draw_pixel(WIDTH-10, HEIGHT-10, GL_GREEN);
    assert(gl_read_pixel(WIDTH-10, HEIGHT-10) == GL_GREEN);

    // Blue rectangle in center of screen
    gl_draw_rect(WIDTH/2 - 100, HEIGHT/2 - 50, 200, 100, GL_BLUE);

    // Single amber character
    gl_draw_char(60, 10, 'A', GL_AMBER);

    // // Show buffer with drawn contents
    gl_swap_buffer();
    pause("Now displaying 1280 x 720, purple bg, single green pixel, blue center rect, amber letter A");
}

static void test_console(void) {
    console_init(25, 50, GL_CYAN, GL_INDIGO);
    pause("Now displaying console: 25 rows x 50 columns, bg indigo, fg cyan");

    // Line 1: Hello, world!
    console_printf("Hello, world!\n");

    // Add line 2: Happiness == CODING
    console_printf("Happiness");
    console_printf(" == ");
    console_printf("CODING\n");

    // Add 2 blank lines and line 5: I am Pi, hear me roar!
    console_printf("\n\nI am Pi, hear me v\b \broar!\n"); // typo, backspace, correction
    pause("Console printfs");

    // Clear all lines
    console_printf("\f");

    // Line 1: "Goodbye"
    console_printf("Goodbye!\n");
    pause("Console clear");
}

/* TODO: Add tests to test your graphics library and console.
   For the graphics library, test both single & double
   buffering and confirm all drawing is clipped to bounds
   of framebuffer
   For the console, make sure to test wrap of long lines and scrolling.
   Be sure to test each module separately as well as in combination
   with others.
*/


static void test_fb_More(void) {
    const int SIZE = 500;
    fb_init(SIZE, SIZE, FB_SINGLEBUFFER); // init single buffer

    assert(fb_get_width() == SIZE);
    assert(fb_get_height() == SIZE);
    assert(fb_get_depth() == 4);
    
    void *cptr = fb_get_draw_buffer();
    assert(cptr != NULL);
    int nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0x99, nbytes); 

    fb_swap_buffer(); //confirm does not do anything and displays light gray pixels
    pause("Confirm fb_swap_buffer() does not affect single buffer - 500x500 gray");

    fb_init(1280, 720, FB_DOUBLEBUFFER); // init double buffer
    pause("Confirm black screen after reset");

    cptr = fb_get_draw_buffer();
    nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0x99, nbytes); 
    fb_swap_buffer();
    pause("Confirm gray screen");

    cptr = fb_get_draw_buffer();
    nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0xFF, nbytes); 
    fb_swap_buffer();
    pause("Confirm white screen swap");
}

static void test_gl_More_2(void){
    const int WIDTH = 800;
    const int HEIGHT = 800;

    // Double buffer mode
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    //test rectangle bounds
    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectanlge in bottom left
    gl_draw_rect(-400,-400,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle out of bounds extend in bounds");


    //test character/string bounds
    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //strings in bottom 
    gl_draw_string(-10,400,"George Washington is a cool guy that lives in Mount Vermon",gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test 'George Washington is a cool guy that lives in Mount Vermon' string bounded bottom");
}


static void test_gl_More(void){
    const int WIDTH = 800;
    const int HEIGHT = 800;

    // Double buffer mode
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    //test rectangle bounds
    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectanlge in bottom left
    gl_draw_rect(400,400,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle bounds bottom left");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectanlge in bottom half
    gl_draw_rect(0,400,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle bounds bottom half");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectanlge in very right
    gl_draw_rect(400,0,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle bounds very right");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectangle with out of bounds x and y
    gl_draw_rect(1600,1600,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle bounds with out of bounds x and y");

    //test character bounds
    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //character in bottom left
    gl_draw_char(792,792,'A',gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test 'A' character bounded bottom left");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //character in bottom half
    gl_draw_char(400,792,'?',gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test '?' character bounded bottom half");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //character in very right
    gl_draw_char(792,400,'6',gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test '6' character bounded very right");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //character placed out of bounds
    gl_draw_char(900,900,'L',gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test character placed out of bounds");

    // Single buffer mode
    gl_init(WIDTH, HEIGHT, GL_SINGLEBUFFER);

    //test rectangle bounds
    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectanlge in bottom left
    gl_draw_rect(400,400,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle bounds bottom left");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectanlge in bottom half
    gl_draw_rect(0,400,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle bounds bottom half");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectanlge in very right
    gl_draw_rect(400,0,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle bounds very right");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //rectangle with out of bounds x and y
    gl_draw_rect(1600,1600,800,800,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test rectangle bounds with out of bounds x and y");

    //test character bounds
    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //character in bottom left
    gl_draw_char(792,792,'A',gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test 'A' character bounded bottom left");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //character in bottom half
    gl_draw_char(400,792,'?',gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test '?' character bounded bottom half");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //character in very right
    gl_draw_char(792,400,'6',gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test '6' character bounded very right");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //character placed out of bounds
    gl_draw_char(900,900,'L',gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test character placed out of bounds");

    // Double buffer mode
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    //test string bounds
    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //strings in bottom 
    gl_draw_string(0,792,"George Washington is a cool guy that lives in Mount Vermon",gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test 'George Washington is a cool guy that lives in Mount Vermon' string bounded bottom");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //string to the right
    gl_draw_string(400,400,"Youtube is an interesting website that piques curiousity of the masses",gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test 'Youtube is an interesting website that piques curiousity of the masses' string bounded right");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //string placed out of bounds
    gl_draw_string(900,900,"Youtube is an interesting website that piques curiousity of the masses",gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test string placed out of bounds");

    // Single buffer mode
    gl_init(WIDTH, HEIGHT, GL_SINGLEBUFFER);

    //test string bounds
    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //strings in bottom 
    gl_draw_string(0,792,"George Washington is a cool guy that lives in Mount Vermon",gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test 'George Washington is a cool guy that lives in Mount Vermon' string bounded bottom");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //string to the right
    gl_draw_string(400,400,"Youtube is an interesting website that piques curiousity of the masses",gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test 'Youtube is an interesting website that piques curiousity of the masses' string bounded right");

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //string placed out of bounds
    gl_draw_string(900,900,"Youtube is an interesting website that piques curiousity of the masses",gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test string placed out of bounds");

    // Single buffer mode
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    assert(gl_read_pixel(900,900)==0); //assert read pixel out of bounds

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //pixel placed out of bounds
    gl_draw_pixel(900,900,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test gl_pixel placed out of bounds");

    // Single buffer mode
    gl_init(WIDTH, HEIGHT, GL_SINGLEBUFFER);

    assert(gl_read_pixel(900,900)==0); //assert read pixel out of bounds

    gl_clear(gl_color(0xFF, 0xFF, 0xFF)); //pixel placed out of bounds
    gl_draw_pixel(900,900,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test gl_pixel placed out of bounds");
}


static void test_consoleMore(void) {
    console_init(25, 50, GL_CYAN, GL_INDIGO);
    pause("Now displaying console: 25 rows x 50 columns, bg indigo, fg cyan");

    console_printf("You ask why I am a person. You should look no further than yourself for the answer to the question \n");
    pause("Test wrapping");

    console_printf("You will get a dog.\nI am a dog.");
    pause("Test newline");

    console_printf("\b\b\b\bduck that likes to quack.");
    pause("Test backspace");

    console_printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bThis is the replacement.");
    pause("Test wrapping and backspace");

    console_printf("\f");
    pause("Test formfeed");

    console_printf("\f \b\b\b");
    pause("Test formfeed and backspace (backspace should do nothing)");

    console_printf("You will get a dog.\n\bI am a dog.");
    pause("Test newline and backspace");

    console_printf("Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.Garfield exists in my cat.");
    pause("Test scrolling and wrapping");
}


void test_line(){
    const int WIDTH = 100;
    const int HEIGHT = 100;

    // Double buffer mode
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(0, 0, 99,99,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(10, 10, 20,40,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(10, 10, 40,90,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(10, 90, 10,10,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(0, 90, 10,10,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    // Double buffer mode
    gl_init(1000, 1000, GL_DOUBLEBUFFER);

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(100, 100, 200,400,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(100, 100, 400,900,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(100, 900, 100,100,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(0, 900, 100,100,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");



    // Double buffer mode
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(10, 10, 40,40,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    //test rectangle bounds
    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(10, 10, 90,40,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(0, 40, 40,10,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    gl_clear(gl_color(0,0,0));  
    gl_draw_line(0, 40, 90,10,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");

    gl_clear(gl_color(0,0,0)); 
    gl_draw_line(0, 0, 90,0,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test line");


    // Double buffer mode
    gl_init(1000, 1000, GL_DOUBLEBUFFER);

     //test rectangle bounds
     gl_clear(gl_color(0,0,0)); 
     gl_draw_line(100, 100, 400,400,gl_color(0x55, 0, 0x55));
     gl_swap_buffer();
     pause("Test line");
 
     //test rectangle bounds
     gl_clear(gl_color(0,0,0)); 
     gl_draw_line(100, 100, 900,400,gl_color(0x55, 0, 0x55));
     gl_swap_buffer();
     pause("Test line");
 
     gl_clear(gl_color(0,0,0)); 
     gl_draw_line(0, 400, 400,100,gl_color(0x55, 0, 0x55));
     gl_swap_buffer();
     pause("Test line");
 
     gl_clear(gl_color(0,0,0));  
     gl_draw_line(0, 400, 900,100,gl_color(0x55, 0, 0x55));
     gl_swap_buffer();
     pause("Test line");
 
     gl_clear(gl_color(0,0,0)); 
     gl_draw_line(0, 0, 900,0,gl_color(0x55, 0, 0x55));
     gl_swap_buffer();
     pause("Test line");

     
}

void test_triangle(){
    const int WIDTH = 100;
    const int HEIGHT = 100;

    // Double buffer mode
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl_clear(gl_color(0,0,0)); 
    gl_draw_triangle(0, 0, 40,40,80,20,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test triangle");

    gl_clear(gl_color(0,0,0)); 
    gl_draw_triangle(0, 0, 0,50,10,20,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test triangle");

    gl_clear(gl_color(0,0,0)); 
    gl_draw_triangle(0, 0, 0,50,99,10,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test triangle");


    // Double buffer mode
    gl_init(1000, 1000, GL_DOUBLEBUFFER);

    long g =  timer_get_ticks()/24;
    gl_clear(gl_color(0,0,0)); 
    gl_draw_triangle(0, 0, 400,400,800,200,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    long new = timer_get_ticks()/24;
    printf("\nTOP %ld",(new-g));
    pause("Test triangle");

    gl_clear(gl_color(0,0,0)); 
    gl_draw_triangle(0, 0, 0,500,990,200,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test triangle");

    gl_clear(gl_color(0,0,0)); 
    gl_draw_triangle(0, 0, 0,500,100,200,gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    pause("Test triangle");
}

void main(void) {
    timer_init();
    uart_init();
    // printf("Executing main() in test_gl_console.c\n");
    // test_fb();
    // test_fb_More();
    // test_gl();
    // test_gl_More();
    test_gl_More_2();
    // test_console();
    // test_consoleMore();
    // test_line();
    // test_triangle();

    printf("Completed main() in test_gl_console.c\n");
}
