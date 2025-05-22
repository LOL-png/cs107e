#include "gl.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "gpio_interrupt.h"
#include "interrupts.h"
#include "printf.h"
#include "ringbuffer.h"
#include "uart.h"

static const gpio_id_t BUTTON = GPIO_PB4;
static volatile int gCount = 0;

void wait_for_click(void) {
    while(gpio_read(BUTTON)==1);
    gCount++;
    uart_putstring("click!");
}

void handle_click(void* auxdata){
    gpio_interrupt_clear(BUTTON);
    gCount++;
    uart_putstring("Howard has interrupt mojo!");
}

void redraw(int nclicks) {
    static int nredraw = -1;
    // count number of redraws, alternate bg color on each redraw
    color_t bg = nredraw++ % 2 ? GL_AMBER : GL_BLUE;

    gl_clear(GL_BLACK);
    char buf[100];
    snprintf(buf, sizeof(buf), "Click count = %d (redraw #%d)", nclicks, nredraw);
    gl_draw_string(0, 0, buf, GL_WHITE);
    // intentionally slow loop for educational purposes :-)
    for (int y = gl_get_char_height(); y < gl_get_height(); y++) {
        for (int x = 0; x < gl_get_width(); x++) {
            gl_draw_pixel(x, y, bg);
        }
    }
}

void config_button(void) {
    gpio_set_input(BUTTON);
    gpio_set_pullup(BUTTON); // use internal pullup resistor
    gpio_interrupt_init();
    gpio_interrupt_config(BUTTON, GPIO_INTERRUPT_NEGATIVE_EDGE, true);
    gpio_interrupt_register_handler(BUTTON, handle_click, NULL);
    gpio_interrupt_enable(BUTTON);
}

void main(void) {
    gpio_init();
    uart_init();
    gl_init(1000, 700, GL_SINGLEBUFFER);
    interrupts_init();
    config_button();
    interrupts_global_enable();

    int drawn = 0;
    redraw(drawn);
    while (true) {
        if(gCount > drawn){
            drawn = gCount;
             // wait_for_click();
            redraw(drawn);
        }
    }
}
