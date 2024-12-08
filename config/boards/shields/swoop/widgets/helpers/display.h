#include <zephyr/kernel.h>

typedef enum {
    CHAR_0,
    CHAR_1,
    CHAR_2,
    CHAR_3,
    CHAR_4,
    CHAR_5,
    CHAR_6,
    CHAR_7,
    CHAR_8,
    CHAR_9,
    CHAR_A,
    CHAR_B,
    CHAR_C,
    CHAR_D,
    CHAR_E,
    CHAR_F,
    CHAR_G,
    CHAR_H,
    CHAR_I,
    CHAR_J,
    CHAR_K,
    CHAR_L,
    CHAR_M,
    CHAR_N,
    CHAR_O,
    CHAR_P,
    CHAR_Q,
    CHAR_R,
    CHAR_S,
    CHAR_T,
    CHAR_U,
    CHAR_V,
    CHAR_W,
    CHAR_X,
    CHAR_Y,
    CHAR_Z,
    CHAR_COLON,
    CHAR_DASH,
    CHAR_PERCENTAGE,
    CHAR_NONE
} Character;

typedef enum {
    TRANSPORT_USB,
    TRANSPORT_BLUETOOTH
} Transport;

typedef enum {
    STATUS_OPEN,
    STATUS_OK,
    STATUS_NOT_OK
} Status;

typedef enum {
    FONT_SIZE_4x6,
    FONT_SIZE_5x7,
    FONT_SIZE_5x8,
} FontSize;

void fill_buffer_color(uint8_t *buf, size_t buf_size, uint32_t color);
void init_display(void);
void display_write_wrapper(uint16_t x, uint16_t y, struct display_buffer_descriptor *buf_desc, uint8_t *buf);
void print_bitmap(uint16_t *scaled_bitmap, Character c, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color, FontSize font_size);
void print_bitmap_status(uint16_t *scaled_bitmap, Status s, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color);
void print_bitmap_transport(uint16_t *scaled_bitmap, Transport t, bool is_ready, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color);
void print_frame(uint8_t *buf_frame, uint16_t start_x, uint16_t end_x, uint16_t start_y, uint16_t end_y);
void print_rectangle(uint8_t *buf_frame, uint16_t start_x, uint16_t end_x, uint16_t start_y, uint16_t end_y, uint16_t color, uint16_t scale);

void set_splash_num_color(uint32_t color);
void set_splash_bg_color(uint32_t color);
void set_snake_font_color(uint32_t color);
void set_snake_num_color(uint32_t color);
void set_snake_bg_color(uint32_t color);
void set_snake_board_color(uint32_t color);
void set_snake_board_1_color(uint32_t color);
void set_snake_default_color(uint32_t color);
void set_food_color(uint32_t color);
void set_snake_color_0(uint32_t color);
void set_snake_color_1(uint32_t color);
void set_snake_color_2(uint32_t color);
void set_snake_color_3(uint32_t color);
void set_snake_color_4(uint32_t color);
void set_snake_color_5(uint32_t color);
void set_snake_color_6(uint32_t color);
void set_battery_num_color(uint32_t color);
void set_battery_bg_color(uint32_t color);
void set_battery_percentage_color(uint32_t color);
void set_frame_color(uint32_t color);
void set_frame_color_1(uint32_t color);
void set_symbol_selected_color(uint32_t color);
void set_symbol_unselected_color(uint32_t color);
void set_symbol_bg_color(uint32_t color);
void set_bt_num_color(uint32_t color);
void set_bt_bg_color(uint32_t color);

uint16_t get_splash_num_color(void);
uint16_t get_splash_bg_color(void);
uint16_t get_snake_font_color(void);
uint16_t get_snake_num_color(void);
uint16_t get_snake_bg_color(void);
uint16_t get_snake_default_color(void);
uint16_t get_snake_board_color(void);
uint16_t get_snake_board_1_color(void);
uint16_t get_food_color(void);
uint16_t get_snake_color_0(void);
uint16_t get_snake_color_1(void);
uint16_t get_snake_color_2(void);
uint16_t get_snake_color_3(void);
uint16_t get_snake_color_4(void);
uint16_t get_snake_color_5(void);
uint16_t get_snake_color_6(void);
uint16_t get_battery_num_color(void);
uint16_t get_battery_bg_color(void);
uint16_t get_battery_percentage_color(void);

uint16_t get_symbol_selected_color(void);
uint16_t get_symbol_unselected_color(void);
uint16_t get_symbol_bg_color(void);
uint16_t get_bt_num_color(void);
uint16_t get_bt_bg_color(void);
uint16_t get_frame_color(void);
uint16_t get_frame_color_1(void);