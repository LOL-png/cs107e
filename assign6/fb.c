/* File: fb.c
 * ----------
 * ***** Framebuffer single and double mode *****
 */
#include "fb.h"
#include "de.h"
#include "hdmi.h"
#include "malloc.h"
#include "strings.h"

// module-level variables, you may add/change this struct as you see fit
static struct {
    int width;             // count of horizontal pixels
    int height;            // count of vertical pixels
    int depth;             // num bytes per pixel
    int frameMode;         // mode of the frame (0 means current is buffOne; 1 means current one in use is buffTwo)
    // int initial;          //set init  
    int mode;              //single or double buff mode
    void *framebufferOne;     // address of framebuffer1 memory
    void *framebufferTwo;     // address of framebuffer2 memory
} module;

void fb_init(int width, int height, fb_mode_t mode) {
    if(module.framebufferOne == module.framebufferTwo){ //reset after first initialization
        free(module.framebufferOne);
    }
    else{
        free(module.framebufferOne);
        free(module.framebufferTwo);
    }

    module.width = width;
    module.height = height;
    module.depth = 4;
    int nbytes = module.width * module.height * module.depth;

    if(mode == FB_SINGLEBUFFER){
        module.mode =  FB_SINGLEBUFFER;
        module.frameMode = 1;  //classify switchMode

        module.framebufferOne = malloc(nbytes); //bufferOne
        module.framebufferTwo = NULL; //bufferTwo

        memset(module.framebufferOne, 0x0, nbytes); //memset bufferOne
        //no memset bufferTwo
    }
    else{
        module.mode =  FB_DOUBLEBUFFER;
        module.frameMode = 0; //classify switchMode

        module.framebufferOne = malloc(nbytes); //bufferOne
        module.framebufferTwo = malloc(nbytes); //bufferTwo

        memset(module.framebufferOne, 0x0, nbytes); //memset bufferOne
        memset(module.framebufferTwo, 0x0, nbytes); //memset bufferTwo
    }

    hdmi_resolution_id_t id = hdmi_best_match(width, height);
    hdmi_init(id);
    de_init(width, height, hdmi_get_screen_width(), hdmi_get_screen_height());
    de_set_active_framebuffer(module.framebufferOne);

}

int fb_get_width(void) {
    return module.width;
}

int fb_get_height(void) {
    return module.height;
}

int fb_get_depth(void) {
    return module.depth;
}

void* fb_get_draw_buffer(void){
   if(module.frameMode == 1){ //if current is buffTwo, return buffOne
        return module.framebufferOne;
   }
   else{ //if current is buffOne, return buffTwo;
        return module.framebufferTwo;
   }
}

void fb_swap_buffer(void) {
    if(module.mode == FB_DOUBLEBUFFER){
        if (module.frameMode == 1){ //if current is buffTwo, display buffOne
            de_set_active_framebuffer(module.framebufferOne);
        }
        else{ //if current is buffOne, display buffTwo
            de_set_active_framebuffer(module.framebufferTwo);
        }
        module.frameMode = module.frameMode ^ 1;
    }
}
