#include "helpers/display.h"

static uint8_t *buf_frame;

void print_frames() {
    // board frame
    print_frame(buf_frame, 0, 239, 0, 319);
    
    // snake stats frames 
    print_frame(buf_frame, 1, 80, 1, 22);
    print_frame(buf_frame, 81, 150, 1, 22);
    print_frame(buf_frame, 151, 238, 1, 22);

    // snake frame
    print_frame(buf_frame, 1, 238, 23, 274);
    
    // battery frames 
    print_frame(buf_frame, 1, 80, 275, 318);
    print_frame(buf_frame, 81, 160, 275, 318);
    print_frame(buf_frame, 161, 238, 275, 318);
}

void zmk_widget_frame_init() {
    buf_frame = (uint8_t*)k_malloc(320 * 2 * sizeof(uint8_t));
}