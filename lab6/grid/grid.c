#include "fb.h"
#include "gl.h"
#include "printf.h"
#include "uart.h"

void draw_pixel(int x, int y, color_t c) {
    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    im[y][x] = c;
}

void draw_hline(int y, color_t c) {
    for (int x = 0; x < fb_get_width(); x++) {
        draw_pixel(x, y, c);
    }
}

void draw_vline(int x, color_t c) {
    for (int y = 0; y < fb_get_height(); y++) {
        draw_pixel(x, y, c);
    }
}

void draw_rect(int x, int y, int width, int height, color_t c){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < height;j++){
            draw_pixel(j+x,i+y, c);
        }
    }
}

void main(void)  {
    uart_init();
    fb_init(800, 600, FB_SINGLEBUFFER); // using fb module from libmango

    // for (int y = 0; y < fb_get_height(); y += 25) {
    //     draw_hline(y, 0xffff0000);
    // }

    // for (int x = 0; x < fb_get_width(); x += 25) {
    //     draw_vline(x, 0xffffff00);
    // }

    int colorState = 0;
    for(int y = 0; y < fb_get_height();y+=25){
        colorState = colorState^1;
        for (int x = 0; x < fb_get_width(); x += 25) {
            if((( (x%2+colorState)%2==0))){
                draw_rect(x,y,25,25, 0xffff0000);
            }
            else{
                draw_rect(x,y,25,25, 0xffffff00);
            }
        }
    }
    printf("Hit any key to quit: ");
    uart_getchar();
    printf("\nCompleted %s\n", __FILE__);
}
