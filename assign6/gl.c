/* File: gl.c
 * ----------
 * ***** This implements graphics for pixels, rectangles, characters, and strings *****
 */
#include "gl.h"
#include "fb.h"
#include "font.h"
#include "malloc.h"
#include "strings.h"

// static int count = 0;
void gl_init(int width, int height, gl_mode_t mode) {
    fb_init(width, height, mode);
    // if(count == 0){
    //     count++;
    //     uint32_t output;
    //     __asm__ __volatile__ ("csrr %0, mstatus"
    //         :"=r"(output)
    //         :
    //         :);
    //     output += 0b1<<13;
    //     __asm__ __volatile__ ("csrw mstatus, %0"
    //                 : 
    //                 :"r"(output)
    //                 : );
    // }
}

int gl_get_width(void) {
    return fb_get_width();
}

int gl_get_height(void) {
    return fb_get_height();
}

color_t gl_color(uint8_t r, uint8_t g, uint8_t b) {
    return (0xFF << 24) + (r << 16) + (g << 8) + b;
}

void gl_swap_buffer(void) {
    fb_swap_buffer();
}

void gl_clear(color_t c) {
    unsigned long totalnum = (( unsigned long) gl_get_height()) *  ( ( unsigned long) gl_get_width());  
    unsigned int *im = fb_get_draw_buffer();
    while (totalnum--){ //copies each byte to destination
        *im = c;
        im++;
    }
}

void gl_draw_pixel(int x, int y, color_t c) {
    if((x >= 0) & (x < fb_get_width()) & (y >= 0)  & (y < fb_get_height())){ //bounds checking
        unsigned int per_row = fb_get_width();
        unsigned int (*im)[per_row] = fb_get_draw_buffer();
        im[y][x] = c;
    }
}

color_t gl_read_pixel(int x, int y) {
    if((x >= 0) & (x < fb_get_width()) & (y >= 0)  & (y < fb_get_height())){ //bounds checking
        unsigned int per_row = fb_get_width();
        unsigned int (*im)[per_row] = fb_get_draw_buffer();
        return (color_t) im[y][x];
    }
    else{
        return 0;
    }
}

void gl_draw_rect(int x, int y, int w, int h, color_t c) {
    int width = gl_get_width();
    int height = gl_get_height();
    unsigned int (*im)[width] = fb_get_draw_buffer();
    if((x < width)& (y < height)){ //only draw if x and y are less than width and height
        if(x < 0){ //rescale for negative x
            w = w + x;
            x = 0;
        }
        if(y < 0){ //rescale for negative y
            h = h + y;
            y = 0;
        }
        
        if( (x + w) > width){ //chuck off extra
            w = width - x;
        }
        if( (y + h) > height){ //chuck off extra
            h = height - y;
        }
        for(int i = 0 ; i < h;i++){ //does nothing for negative height and widths
            for(int j = 0; j < w;j++){
                im[y+i][x+j] = c;
            }
        }
    }
}

void gl_draw_char(int x, int y, char ch, color_t c) {
    int width = gl_get_width();
    int height = gl_get_height();

    if((x < width) & (y < height)){  //only draw if x and y are less than width and height
        size_t bufsize= font_get_glyph_size();
        uint8_t *buf = malloc(bufsize);

        if(!font_get_glyph(ch,buf,bufsize)){ //if it failed to get a font
            free(buf); //release memory
            return;
        }

        int w = font_get_glyph_width();
        int h = font_get_glyph_height();

        uint8_t (*fontMap)[w]  = ((void * )buf); //font map
        unsigned int (*im)[width] = fb_get_draw_buffer(); //unsigned load buffer

        int jstart = 0;
        int istart = 0;
        if(x < 0){ //rescale for negative x 
            jstart = -1*x;
            x = 0;
        }

        if(y < 0){ //rescale for negative y
            istart = -1*y;
            y = 0;
        }
    
        if( (x + w) > width){ //chuck off extra
            w = width - x;
        }
        if( (y + h) > height){ //chuck off extra
            h = height - y;
        }

        for(int i = istart ; i < h;i++){ //start at istart, jstart - will clip as necessary
            for(int j = jstart; j < w;j++){ //add color of c if the fontMap deems so
                if(fontMap[i][j]){
                    im[y+i-istart][x+j-jstart] = c;
                }
            }
        }

        free(buf); //release memory
    }
}
void gl_draw_string(int x, int y, const char* str, color_t c) {
    int width = font_get_glyph_width();
    for(int i = 0; i < strlen(str);i++){ //loop to draw each character
        gl_draw_char(x+width*i,y,str[i],c);
    }
}

// int higherLowerLine(float a, float b){
    
// }

// float fabs(float f) {
//    if (f < 0){
//         return -1*f;
//    }
//    else{
//         return f;
//    }
// }

void gl_draw_line(int x1, int y1, int x2, int y2, color_t c){
    // int width = gl_get_width();
    // int height = gl_get_height();
    
    // if((x1 >= 0) & (x1 < width) & (y1 >= 0)  & (y1 < height)){ //boundary check;

    // }
    // else{
    //     return;
    // }

    // if((x2 >= 0) & (x2 < width) & (y2 >= 0)  & (y2 < height)){ //boundary check;
        
    // }
    // else{
    //     return;
    // }

    // //flip x and y if x2 < x1
    // if(x2 < x1){
    //     int tempX2 = x2;
    //     int tempX1 = x1;
    //     int tempY1 = y1;
    //     int tempY2 = y2;

    //     x2 = tempX1;
    //     x1 = tempX2;
    //     y2 = tempY1;
    //     y1 = tempY2;
    // }

    // //draw first and last pixels
    // gl_draw_pixel(x1, y1, c);
    // gl_draw_pixel(x2, y2,c);

    // if( ((y2 > y1) & ((y2 - y1) > (x2 -x1))) | ((y1 > y2) & ((y1 - y2) > (x2 -x1)))){ //if our y is greater than x, we implement the dx/dy
    //     //flip x and y if y2 < x1
    //     if(y2 < y1){
    //         int tempX2 = x2;
    //         int tempX1 = x1;
    //         int tempY1 = y1;
    //         int tempY2 = y2;

    //         x2 = tempX1;
    //         x1 = tempX2;
    //         y2 = tempY1;
    //         y1 = tempY2;
    //     }
        
    //     //slope
    //     float m = ( ((float) x2)-  ((float) x1))/( ((float) y2) - ((float) y1));
   
    //      //current y value (set as 0.5 for the middle)
    //     float ycurr = ((float) x1) + 0.5;
    //     int yprev = 0;

    //     if(m >= 0){
    //         yprev = x1 + 1;
    //     }

    //     else{
    //         yprev = x1-1;
    //     }

    //      for(int i = y1+1;i < y2;i++){
    //         ycurr+=m;

    //         int xnew = i;
    //         int y1new = 0;
    //         int y2new = 0;

    //         if(m >= 0){
    //             y1new = yprev-1;
    //             y2new = yprev;
    //         }
    //         else{
    //             y1new = yprev+1;
    //             y2new = yprev;
    //         }

    //         if(m >= 0){
    //             if( ycurr > ((float) yprev +0.5)){//change of points
    //                 yprev = (int) ycurr+1;
    //                 y2new++;
    //                 y1new++;
    //             }

    //             if(y1new > x2){
    //                 y1new = x2;
    //             }
    //             if(y2new > x2){
    //                 y2new = x2;
    //             }
    //         }
    //         else{
    //             if(ycurr < ((float) yprev + 0.5)){//change of points
    //                 yprev = (int) ycurr;
    //                 y2new--;
    //                 y1new--;
    //             }

    //             if(y1new < x2){
    //                 y1new = x2;
    //             }
    //             if(y2new < x2){
    //                 y2new = x2;
    //             }
    //         }

    //         float dist1 = fabs( ((float) y1new) + 0.5 - ycurr); //compute distance
    //         float dist2 = fabs( ((float) y2new) + 0.5 - ycurr);

    //         if(dist1 > 1){ //invert distance for brightness
    //             dist1 = 1;
    //         }

    //         if(dist2 > 1){
    //             dist2 = 1;
    //         }

    //         dist1 = (1-dist1);
    //         dist2 = (1-dist2);

    //         color_t c1 =   (unsigned int) (dist1*((float) (c & 0xFF))) //obtain color code
    //         +  (((unsigned int) (dist1*((float) ( (c >> 8) & 0xFF)))) << 8 )
    //         +  (((unsigned int) (dist1*((float) ( (c >> 16) & 0xFF)))) << 16)
    //         +  ( ((unsigned int) (0xFF)) << 24);

    //         color_t c2 =   (unsigned int) (dist2*((float) (c & 0xFF)))
    //         +  (((unsigned int) (dist2*((float) ( (c >> 8) & 0xFF)))) << 8)
    //         +  (((unsigned int) (dist2*((float) ( (c >> 16) & 0xFF)))) << 16)
    //         +  ( ((unsigned int) (0xFF)) << 24);

  
    //         gl_draw_pixel(y1new, xnew, c1);
    //         gl_draw_pixel(y2new, xnew,c2);
            
        
    //     }
    // }
    // else{ //if our x is greater than y, we implement dy/dx

    //     //slope
    //     float m = ( ((float) y2)-  ((float) y1))/( ((float) x2) - ((float) x1));
   
    //      //current y value (set as 0.5 for the middle)
    //     float ycurr = ((float) y1) + 0.5;
    //     int yprev = 0;

    //     if(m >= 0){
    //         yprev = y1 + 1;
    //     }

    //     else{
    //         yprev = y1-1;
    //     }

    //      for(int i = x1+1;i < x2;i++){
    //         ycurr+=m;

    //         int xnew = i;
    //         int y1new = 0;
    //         int y2new = 0;

    //         if(m >= 0){
    //             y1new = yprev-1;
    //             y2new = yprev;
    //         }
    //         else{
    //             y1new = yprev+1;
    //             y2new = yprev;
    //         }

    //         if(m >= 0){
    //             if( ycurr > ((float) yprev +0.5)){ //change of points
    //                 yprev = (int) ycurr+1;
    //                 y2new++;
    //                 y1new++;
    //             }

    //             if(y1new > y2){
    //                 y1new = y2;
    //             }
    //             if(y2new > y2){
    //                 y2new = y2;
    //             }
    //         }
    //         else{
    //             if(ycurr < ((float) yprev + 0.5)){ //change of points
    //                 yprev = (int) ycurr;
    //                 y2new--;
    //                 y1new--;
    //             }

    //             if(y1new < y2){
    //                 y1new = y2;
    //             }
    //             if(y2new < y2){
    //                 y2new = y2;
    //             }
    //         }

    //         float dist1 = fabs( ((float) y1new) + 0.5 - ycurr); //compute distance
    //         float dist2 = fabs( ((float) y2new) + 0.5 - ycurr);

    //         if(dist1 > 1){ //invert distance for brightness
    //             dist1 = 1;
    //         }

    //         if(dist2 > 1){
    //             dist2 = 1;
    //         }

    //         dist1 = (1-dist1);
    //         dist2 = (1-dist2);

    //         color_t c1 =   (unsigned int) (dist1*((float) (c & 0xFF))) //obtain color code
    //         +  (((unsigned int) (dist1*((float) ( (c >> 8) & 0xFF)))) << 8 )
    //         +  (((unsigned int) (dist1*((float) ( (c >> 16) & 0xFF)))) << 16)
    //         +  ( ((unsigned int) (0xFF)) << 24);

    //         color_t c2 =   (unsigned int) (dist2*((float) (c & 0xFF)))
    //         +  (((unsigned int) (dist2*((float) ( (c >> 8) & 0xFF)))) << 8)
    //         +  (((unsigned int) (dist2*((float) ( (c >> 16) & 0xFF)))) << 16)
    //         +  ( ((unsigned int) (0xFF)) << 24);

  
    //         gl_draw_pixel(xnew, y1new, c1); //draw
    //         gl_draw_pixel(xnew, y2new,c2);
            
        
    //     }
    // }


}


// int* interpolate(int x1, int y1, int x2, int y2){
    // int bufsize = y2-y1+1;
    // int * array= malloc(bufsize*sizeof(int));
    // float m = ( ((float) x2)-  ((float) x1))/( ((float) y2) - ((float) y1)); //slope
    // float sloapBuild = x1 + m;

    // array[0] = x1;
    // array[bufsize-1] = x2;
    // for(int i = 1; i < bufsize-1;i++){
    //     array[i] = (int) sloapBuild;
    //     sloapBuild+=m;
    // }
    
    // return array;
// }
void gl_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color_t c){
    // if((x1 == x2) & (y1 == y2) ){ //boundary check;
    //     return;
    // }

    // if((x2 == x3) & (y2 == y3) ){ //boundary check;
    //     return;
    // }   

    // if((x1 == x3) & (y1 == y3) ){ //boundary check;
    //     return;
    // }


    // gl_draw_line(x1,y1,x2,y2,c); //straight lines
    // gl_draw_line(x2,y2,x3,y3,c); 
    // gl_draw_line(x1,y1,x3,y3,c); 


    // if(y2 < y1){ //swap points
    //     int tempX2 = x2;
    //     int tempX1 = x1;
    //     int tempY1 = y1;
    //     int tempY2 = y2;

    //     x2 = tempX1;
    //     x1 = tempX2;
    //     y2 = tempY1;
    //     y1 = tempY2;    
    // }

    // if(y3 < y1){
    //     int tempX3 = x3;
    //     int tempX1 = x1;
    //     int tempY1 = y1;
    //     int tempY3 = y3;

    //     x3 = tempX1;
    //     x1 = tempX3;
    //     y3 = tempY1;
    //     y1 = tempY3;  
    // }

    // if(y3 < y2){
    //     int tempX3 = x3;
    //     int tempX2 = x2;
    //     int tempY2 = y2;
    //     int tempY3 = y3;

    //     x3 = tempX2;
    //     x2 = tempX3;
    //     y3 = tempY2;
    //     y2 = tempY3;  
    // }

    // int sizex12minus = (y2-y1);
    // int sizex23= (y3-y2)+1;
    // int sizex13= (y3-y1)+1;

    // int * x12 = interpolate(x1,y1,x2,y2);
    // int * x23 = interpolate(x2,y2,x3,y3);
    // int * x13 = interpolate(x1,y1,x3,y3);

    // int * x123 = malloc(  (sizex12minus + sizex23) *sizeof(int)); //concatnate
    
    // int n = 0;

    // for(; n < sizex12minus;n++){
    //     x123[n] = x12[n];
    // }

    // for(int i = 0; i < sizex23;i++){
    //     x123[n] = x23[i];
    //     n++;
    // }
    
    // int m  = (int) ((float) (sizex13))/((float)2);

    // int * xleft;
    // int * xright;

    // if( x13[m] < x123[m]){
    //     xleft = x13;
    //     xright = x123;
    // }
    // else{
    //     xleft = x123;
    //     xright = x13;
    // }


    // for(int i = 0; i < (sizex12minus + sizex23);i++){
    //     gl_draw_line(xleft[i], y1+i,  xright[i],y1+i, c);
    // }

    // free(x12);
    // free(x23);
    // free(x13);
    // free(x123);
}


int gl_get_char_height(void) {
    return font_get_glyph_height();
}

int gl_get_char_width(void) {
    return font_get_glyph_width();
}
