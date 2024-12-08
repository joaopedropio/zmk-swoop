#include "helpers/display.h"

static uint8_t *buf_frame;

void print_frames() {
    // board frame
    print_rectangle(buf_frame, 0, 239, 0, 319, get_frame_color(), 1);
    
    // snake stats frames 
    print_rectangle(buf_frame, 1, 84, 1, 22, get_frame_color(), 1);
    print_rectangle(buf_frame, 2, 83, 2, 21, get_frame_color_1(), 1);
    print_rectangle(buf_frame, 85, 157, 1, 22, get_frame_color(), 1);
    print_rectangle(buf_frame, 86, 156, 2, 21, get_frame_color_1(), 1);
    print_rectangle(buf_frame, 158, 238, 1, 22, get_frame_color(), 1);
    print_rectangle(buf_frame, 159, 237, 2, 21, get_frame_color_1(), 1);

    // snake frame
    print_rectangle(buf_frame, 2, 234, 24, 269, get_frame_color_1(), 5);
    print_rectangle(buf_frame, 1, 238, 23, 274, get_frame_color(), 1);
    
    // battery frames 
    print_rectangle(buf_frame, 1, 80, 275, 318, get_frame_color(), 1);
    print_rectangle(buf_frame, 2, 79, 276, 317, get_frame_color_1(), 1);
    print_rectangle(buf_frame, 81, 160, 275, 318, get_frame_color(), 1);
    print_rectangle(buf_frame, 82, 159, 276, 317, get_frame_color_1(), 1);
    print_rectangle(buf_frame, 161, 238, 275, 318, get_frame_color(), 1);
    print_rectangle(buf_frame, 162, 237, 276, 317, get_frame_color_1(), 1);
}

void zmk_widget_frame_init() {
    buf_frame = (uint8_t*)k_malloc(320 * 2 * sizeof(uint8_t));
}