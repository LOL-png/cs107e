/* File: console.c
 * ---------------
 * ***** Console that prints to screen *****
 */
#include "console.h"
#include "gl.h"
#include "malloc.h"
#include "printf.h"
#include "timer.h"

// module-level variables, you may add/change this struct as you see fit!
static struct {
    color_t bg_color, fg_color;
    int initial;
    int line_height;
    int line_width;
    int ncols;
    int nrows;
    int index;
    char * array;
} module;

void console_init(int nrows, int ncols, color_t foreground, color_t background) {
    if(module.initial == 0 ){
        module.initial++;
    }
    else{
        free(module.array);
    }

    // Please add this amount of vertical space between console rows
    const static int LINE_SPACING = 5;

    module.line_height = gl_get_char_height() + LINE_SPACING;
    module.line_width = gl_get_char_width()*ncols;
    module.ncols = ncols;
    module.nrows = nrows;
    module.array = malloc(ncols*nrows+1); //1 extra space for null character

    module.index = 0;

    module.fg_color = foreground;
    module.bg_color = background;
   
    gl_init(module.line_width,module.line_height*nrows, 1);
    gl_clear(background);
    fb_swap_buffer();
    gl_clear(background);
    // fb_swap_buffer();

    //startup screen
    console_startup_screen();
    gl_clear(background);
    fb_swap_buffer();
    gl_clear(background);
}

//clear console
void console_clear(void) {
    module.index = 0;
    gl_clear(module.bg_color);
    fb_swap_buffer();
    gl_clear(module.bg_color);
    // fb_swap_buffer();
}

//scroll movement
void scrollFunct(void){
    char * newBeginning = malloc(module.nrows*module.ncols+1);
    for(int i = 0; i < ((module.nrows-1)*module.ncols);i++){
        newBeginning[i] = module.array[i+module.ncols];
    }
    free(module.array);
    module.index -= module.ncols;
    module.array = newBeginning;
}

//print and wrap simultanously
unsigned int wrapPrint(unsigned int change, unsigned int originalIndex){
    if(change){ //reprint whole screen
        gl_clear(module.bg_color);
        // fb_swap_buffer();
        // gl_clear(module.bg_color);
        // fb_swap_buffer();
        unsigned int n = 0;
        unsigned int x = 0;
        int y = -1*module.line_height;
        while(module.array[n]!='\0'){
            if(n % module.ncols == 0){
                y = y + module.line_height;
                x = 0;
            }
            else{
                x += gl_get_char_width();
            }

            if(module.array[n] != ' '){
                gl_draw_char(x,y,module.array[n],module.fg_color);
            }
            // fb_swap_buffer();
            // gl_draw_char(x,y,module.array[n],module.fg_color);
            // fb_swap_buffer();
            n++;
        }

        fb_swap_buffer();

        gl_clear(module.bg_color);
        n = 0;
        x = 0;
        y = -1*module.line_height;
        while(module.array[n]!='\0'){
            if(n % module.ncols == 0){
                y = y + module.line_height;
                x = 0;
            }
            else{
                x += gl_get_char_width();
            }

            if(module.array[n] != ' '){
                gl_draw_char(x,y,module.array[n],module.fg_color);
            }
            // fb_swap_buffer();
            // gl_draw_char(x,y,module.array[n],module.fg_color);
            // fb_swap_buffer();
            n++;
        }

        if(n==0){
            return 0;
        }
        else{
            return n-1;
        }
    }
    else{ //reprint starting from an index
        unsigned int n = originalIndex;
        unsigned int x = (originalIndex%module.ncols)*gl_get_char_width();
        int y = ( ((int) originalIndex)/((int)module.ncols))*module.line_height;
        if(n % module.ncols == 0){
            y -= module.line_height;
        }
        else{
            x-= gl_get_char_width();
        }
        while(module.array[n]!='\0'){
            if(n % module.ncols == 0){
                y = y + module.line_height;
                x = 0;
            }
            else{
                x += gl_get_char_width();
            }

            if(module.array[n] != ' '){
                gl_draw_char(x,y,module.array[n],module.fg_color);
            }
            // fb_swap_buffer();
            // gl_draw_char(x,y,module.array[n],module.fg_color);
            // fb_swap_buffer();
            n++;
        }

        fb_swap_buffer();

        n = originalIndex;
        x = (originalIndex%module.ncols)*gl_get_char_width();
        y = ( ((int) originalIndex)/((int)module.ncols))*module.line_height;
        if(n % module.ncols == 0){
            y -= module.line_height;
        }
        else{
            x-= gl_get_char_width();
        }
        while(module.array[n]!='\0'){
            if(n % module.ncols == 0){
                y = y + module.line_height;
                x = 0;
            }
            else{
                x += gl_get_char_width();
            }

            if(module.array[n] != ' '){
                gl_draw_char(x,y,module.array[n],module.fg_color);
            }
            // fb_swap_buffer();
            // gl_draw_char(x,y,module.array[n],module.fg_color);
            // fb_swap_buffer();
            n++;
        }

        if(n==0){
            return originalIndex;
        }
        else{
            return n+originalIndex-1;
        }
    }
}

//backspace clear
void backspace_clear(unsigned int originalIndex){
    unsigned int x = (originalIndex%module.ncols)*gl_get_char_width();
    int y = ( ((int) originalIndex)/((int)module.ncols))*module.line_height;
    gl_draw_rect(x,y,gl_get_char_width(),module.line_height,module.bg_color);
    fb_swap_buffer();
    gl_draw_rect(x,y,gl_get_char_width(),module.line_height,module.bg_color);
    // fb_swap_buffer();
}

int console_printf(const char *format, ...) {

    //store printf values
    unsigned int bufsize = (module.ncols*module.nrows)*2+1;
    char buf[bufsize];
    va_list ap; //initalize va_list
    va_start(ap,format); 
    vsnprintf(buf, bufsize, format, ap);
    va_end(ap);

    unsigned int change = 0;
    unsigned int originalIndex = module.index;
    // printf("%s\n",buf);

    //character processing
    unsigned int n = 0;

    while(buf[n]!='\0'){
        if(module.index == module.ncols*module.nrows){
            change = 1;
            scrollFunct();
        }

        if( (buf[n] == 0x8)){ //backspace
            if( (module.index) > 0){ //only backspace if space exists
                module.index--; //return to previous index to overwrite
                module.array[module.index] = ' '; //create space
                if(module.index < originalIndex){
                    originalIndex = module.index;
                    backspace_clear(originalIndex);//initiate backspace clear
                }
            }
            n++; //increment place in buf
            continue;
        }
        if ( buf[n] == 0xC){ //formfeed
            console_clear();
            change = 1;
            n++; //increment place in buf
            continue;
        }
        if( buf[n] ==0xA){ //new line
            int limit = module.ncols - module.index%module.ncols; //find remaining space in the line
            // if( ((module.index%module.ncols) == 0) & (module.index > 0)){
            //     limit = 0;
            // }
            for(int i = 0; i < limit;i++){ //fill in remaining space with spaces
                module.array[module.index] = ' ';
                module.index++;
            }
            n++;  //increment place in buf
            continue;
        }

        module.array[module.index] = buf[n];

        module.index++;
        n++;
    }

    module.array[module.index] = '\0';

    //wrapping - handled as a "compiler time process"
    unsigned int realOutput = wrapPrint(change, originalIndex);
    
    return realOutput;
}


void console_startup_screen(void) {
//    int height = module.line_height*module.nrows;
// //    int centerwidth = module.line_width/2;
// //    int centerheight =height/2;

//     int n = 0;
//     while(n<gl_get_char_width()){
//         for(int i = 0; i < module.ncols;i++){ //begin animation
//             gl_draw_line(i*gl_get_char_width()+n,0,(i)*gl_get_char_width()+n,height-1, gl_color(0xFF, 0xFF, 0xFF));
//             fb_swap_buffer();
//             gl_draw_line(i*gl_get_char_width()+n,0,(i)*gl_get_char_width()+n,height-1, gl_color(0xFF, 0xFF, 0xFF));
//             fb_swap_buffer();
//         }
//         n++;
//     }

//     n = height-module.line_height;
//     while(n>0){
//         gl_draw_rect(0,n,module.line_width, module.line_height,gl_color(0x00, 0x00, 0x00));
//         fb_swap_buffer();
//         gl_draw_rect(0,n,module.line_width, module.line_height,gl_color(0x00, 0x00, 0x00));
//         fb_swap_buffer();
//         n-=module.line_height;
//         if(n < 0){
//             n = 0;
//         }
//     }
   
}
