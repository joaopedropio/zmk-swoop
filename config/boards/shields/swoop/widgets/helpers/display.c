// #include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include "display.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const struct device *display_dev;

static uint16_t splash_num_color;
static uint16_t splash_bg_color;

static uint16_t snake_num_color;
static uint16_t snake_bg_color;

static uint16_t snake_color_0;
static uint16_t snake_color_1;
static uint16_t snake_color_2;
static uint16_t snake_color_3;
static uint16_t snake_color_4;
static uint16_t snake_color_5;
static uint16_t snake_color_6;

static uint16_t battery_num_color;
static uint16_t battery_bg_color;

static uint16_t symbol_selected_color;
static uint16_t symbol_unselected_color;
static uint16_t symbol_bg_color;
static uint16_t bt_num_color;
static uint16_t bt_bg_color;

static uint16_t frame_color;

// uint16_t rgb888_to_rgb565(uint32_t color){
//     uint8_t r = ((color & 0xff0000) / 0x10000);
//     uint8_t g = ((color & 0x00ff00) / 0x100);
//     uint8_t b = ((color & 0x0000ff) / 0x1);
//     uint8_t blue  = (uint16_t)((b >> 3) & 0x1FU); //8 - 3 = 5
//     uint8_t green = (uint16_t)((g >> 2) & 0x3FU); //8 - 2 = 6
//     uint8_t red   = (uint16_t)((r >> 3) & 0x1FU); //8 - 3 = 5
//     return (red << (16-5)) | (green << (16-(5+6))) | blue << (16-(5+6+5));
// }

uint16_t rgb888_to_rgb565(uint32_t color) {
    uint16_t red = (((color & 0xff0000) / 0x10000) * 31 / 255);
    uint16_t green = (((color & 0x00ff00) / 0x100) * 63 / 255);
    uint16_t blue = (((color & 0x0000ff) / 0x1) * 31 / 255);
    
    
    
    // Shift the red value to the left by 11 bits.
    uint16_t red_shifted = red << 11;
    // Shift the green value to the left by 5 bits.
    uint16_t green_shifted = green << 5;

    // Combine the red, green, and blue values.
    return red_shifted | green_shifted | blue;
}

void set_splash_num_color(uint32_t color) {
    splash_num_color = rgb888_to_rgb565(color);
}
void set_splash_bg_color(uint32_t color) {
    splash_bg_color = rgb888_to_rgb565(color);
}

void set_snake_num_color(uint32_t color) {
    snake_num_color = rgb888_to_rgb565(color);
}

void set_snake_bg_color(uint32_t color) {
    snake_bg_color = rgb888_to_rgb565(color);
}

void set_snake_color_0(uint32_t color) {
    snake_color_0 = rgb888_to_rgb565(color);
}

void set_snake_color_1(uint32_t color) {
    snake_color_1 = rgb888_to_rgb565(color);
}

void set_snake_color_2(uint32_t color) {
    snake_color_2 = rgb888_to_rgb565(color);
}

void set_snake_color_3(uint32_t color) {
    snake_color_3 = rgb888_to_rgb565(color);
}

void set_snake_color_4(uint32_t color) {
    snake_color_4 = rgb888_to_rgb565(color);
}

void set_snake_color_5(uint32_t color) {
    snake_color_5 = rgb888_to_rgb565(color);
}

void set_snake_color_6(uint32_t color) {
    snake_color_6 = rgb888_to_rgb565(color);
}

void set_battery_num_color(uint32_t color) {
    battery_num_color = rgb888_to_rgb565(color);
}

void set_battery_bg_color(uint32_t color) {
    battery_bg_color = rgb888_to_rgb565(color);
}

void set_frame_color(uint32_t color) {
    frame_color = rgb888_to_rgb565(color);
}

void set_symbol_selected_color(uint32_t color) {
    symbol_selected_color = rgb888_to_rgb565(color);
}

void set_symbol_unselected_color(uint32_t color) {
    symbol_unselected_color = rgb888_to_rgb565(color);
}

void set_symbol_bg_color(uint32_t color) {
    symbol_bg_color = rgb888_to_rgb565(color);
}

void set_bt_num_color(uint32_t color) {
    bt_num_color = rgb888_to_rgb565(color);
}

void set_bt_bg_color(uint32_t color) {
    bt_bg_color = rgb888_to_rgb565(color);
}

uint16_t get_splash_num_color() {
    return splash_num_color;
}

uint16_t get_splash_bg_color() {
    return splash_bg_color;
}

uint16_t get_snake_num_color() {
    return snake_num_color;
}

uint16_t get_snake_bg_color() {
    return snake_bg_color;
}

uint16_t get_snake_color_0() {
    return snake_color_0;
}

uint16_t get_snake_color_1() {
    return snake_color_1;
}

uint16_t get_snake_color_2() {
    return snake_color_2;
}

uint16_t get_snake_color_3() {
    return snake_color_3;
}

uint16_t get_snake_color_4() {
    return snake_color_4;
}

uint16_t get_snake_color_5() {
    return snake_color_5;
}

uint16_t get_snake_color_6() {
    return snake_color_6;
}

uint16_t get_battery_num_color() {
    return battery_num_color;
}

uint16_t get_battery_bg_color() {
    return battery_bg_color;
}

uint16_t get_symbol_selected_color() {
    return symbol_selected_color;
}

uint16_t get_symbol_unselected_color() {
    return symbol_unselected_color;
}

uint16_t get_symbol_bg_color() {
    return symbol_bg_color;
}

uint16_t get_bt_num_color() {
    return bt_num_color;
}

uint16_t get_bt_bg_color() {
    return bt_bg_color;
}

void display_write_wrapper(uint16_t x, uint16_t y, struct display_buffer_descriptor *buf_desc, uint8_t *buf) {
    display_write(display_dev, x, y, buf_desc, buf);
}

void init_display(void) {
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device %s not found. Aborting sample.", display_dev->name);
		return;
	}
    // LOG_INF("malloc vertical and horizontal line buffers");
    // vertical_line_buf = (uint8_t*)k_malloc(320 * 2 * sizeof(uint8_t));
    // horizontal_line_buf = (uint8_t*)k_malloc(320 * 2 * sizeof(uint8_t));
}

void fill_buffer_color(uint8_t *buf, size_t buf_size, uint32_t color) {
	for (size_t idx = 0; idx < buf_size; idx += 2) {
		*(buf + idx + 0) = (color >> 8) & 0xFFu;
		*(buf + idx + 1) = (color >> 0) & 0xFFu;
	}
}

uint16_t swap_16_bit_color(uint16_t color) {
    return (color >> 8) | (color << 8);
}

void render_bitmap(uint16_t *scaled_bitmap, uint16_t bitmap[], uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t scale, uint16_t num_color, uint16_t bg_color) {
	struct display_buffer_descriptor buf_font_desc;

    uint16_t color;
    uint16_t pixel;
    uint16_t font_width_scaled = width * scale;
    uint16_t font_height_scaled = height * scale;
    uint16_t font_buf_size_scaled = font_width_scaled * font_height_scaled;
    uint16_t index = 0;
    for (uint16_t line = 0; line < height; line++) {
        for (uint16_t i = 0; i < scale; i++) {
            for (uint16_t column = 0; column < width; column++) {
                for (uint16_t j = 0; j < scale; j++) {
                    pixel = bitmap[(line*width) + column];
                    if (pixel == 1) {
                        color = num_color;
                    } else {
                        color = bg_color;
                    }
                    *(scaled_bitmap + index) = swap_16_bit_color(color);
                    index++;
                }
            }
        }
    }
	buf_font_desc.buf_size = font_buf_size_scaled;
	buf_font_desc.pitch = font_width_scaled;
	buf_font_desc.width = font_width_scaled;
	buf_font_desc.height = font_height_scaled;
    display_write(display_dev, x, y, &buf_font_desc, scaled_bitmap);
}

void print_bitmap_5x8(uint16_t *scaled_bitmap, Character c, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color) {
    uint8_t font_width = 5;
    uint8_t font_height = 8;
    uint16_t none_bitmap[] = {
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
    };
    uint16_t dash_bitmap[] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
    };
    uint16_t f_bitmap[] = {
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
    };
    uint16_t u_bitmap[] = {
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 1,
    };
    uint16_t l_bitmap[] = {
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
    };
    uint16_t percentage_bitmap[] = {
        1, 1, 0, 0, 1,
        1, 1, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 0, 0,
        0, 1, 0, 1, 1,
        1, 0, 0, 1, 1,
    };
    uint16_t num_bitmaps[10][40] = {
        {// zero
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// one
            0, 0, 1, 0, 0,
            0, 1, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 1, 1, 1, 0
        },
        {// two
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            0, 0, 0, 0, 1,
            0, 0, 0, 1, 0,
            0, 0, 1, 0, 0,
            0, 1, 0, 0, 0,
            1, 0, 0, 0, 0,
            1, 1, 1, 1, 1
        },
        {// three
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            0, 0, 0, 0, 1,
            0, 0, 1, 1, 0,
            0, 0, 0, 0, 1,
            0, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// four
            0, 0, 0, 1, 0,
            0, 0, 1, 1, 0,
            0, 1, 0, 1, 0,
            1, 0, 0, 1, 0,
            1, 1, 1, 1, 1,
            0, 0, 0, 1, 0,
            0, 0, 0, 1, 0,
            0, 0, 0, 1, 0
        },
        {// five
            1, 1, 1, 1, 1,
            1, 0, 0, 0, 0,
            1, 0, 0, 0, 0,
            1, 1, 1, 1, 0,
            0, 0, 0, 0, 1,
            0, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// six
            0, 0, 1, 1, 0,
            0, 1, 0, 0, 0,
            1, 0, 0, 0, 0,
            1, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// seven
            1, 1, 1, 1, 1,
            0, 0, 0, 0, 1,
            0, 0, 0, 1, 0,
            0, 0, 0, 1, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 1, 0, 0, 0,
            0, 1, 0, 0, 0
        },
        {// eight
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// nine
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 1,
            0, 0, 0, 0, 1,
            0, 0, 0, 1, 0,
            1, 1, 1, 0, 0
        },
    };
    if (c >= 0 && c < 10) {
        render_bitmap(scaled_bitmap, num_bitmaps[c], x, y, font_width, font_height, scale, color, bg_color);
        return;
    }
    switch (c) {
    case CHAR_F: render_bitmap(scaled_bitmap, f_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_U: render_bitmap(scaled_bitmap, u_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_L: render_bitmap(scaled_bitmap, l_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_DASH: render_bitmap(scaled_bitmap, dash_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_PERCENTAGE: render_bitmap(scaled_bitmap, percentage_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    default: render_bitmap(scaled_bitmap, none_bitmap, x, y, font_width, font_height, scale, color, bg_color);
    }
}

void print_bitmap_5x7(uint16_t *scaled_bitmap, Character c, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color) {
    uint8_t font_width = 5;
    uint8_t font_height = 7;
    uint16_t none_bitmap[] = {
        0, 1, 1, 1, 0,
        1, 0, 0, 0, 1,
        0, 0, 0, 0, 1,
        0, 0, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
    };
    uint16_t b_bitmap[] = {
        1, 1, 1, 1, 0,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 0,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 0,
    };
    uint16_t e_bitmap[] = {
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 1, 1, 1, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
    };
    uint16_t l_bitmap[] = {
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
    };
    uint16_t m_bitmap[] = {
        0, 1, 0, 1, 0,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 0, 0, 1,
    };
    uint16_t n_bitmap[] = {
        1, 0, 0, 0, 1,
        1, 1, 0, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 0, 1, 1,
        1, 0, 0, 1, 1,
        1, 0, 0, 0, 1,
    };
    uint16_t p_bitmap[] = {
        1, 1, 1, 1, 0,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
    };
    uint16_t s_bitmap[] = {
        0, 1, 1, 1, 1,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 1,
        0, 0, 0, 0, 1,
        1, 1, 1, 1, 0,
    };
    uint16_t t_bitmap[] = {
        1, 1, 1, 1, 1,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
    };
    uint16_t w_bitmap[] = {
        1, 0, 0, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 1, 0, 1,
        0, 1, 0, 1, 0,
    };
    uint16_t colon_bitmap[] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 0, 0,
    };
    uint16_t num_bitmaps[10][35] = {
        {// zero
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// one
            0, 0, 1, 0, 0,
            0, 1, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 1, 1, 1, 0
        },
        {// two
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            0, 0, 0, 0, 1,
            0, 0, 0, 1, 0,
            0, 0, 1, 0, 0,
            0, 1, 0, 0, 0,
            1, 1, 1, 1, 1
        },
        {// three
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            0, 0, 0, 0, 1,
            0, 0, 1, 1, 0,
            0, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// four
            0, 0, 0, 1, 0,
            0, 0, 1, 1, 0,
            0, 1, 0, 1, 0,
            1, 0, 0, 1, 0,
            1, 1, 1, 1, 1,
            0, 0, 0, 1, 0,
            0, 0, 0, 1, 0
        },
        {// five
            1, 1, 1, 1, 1,
            1, 0, 0, 0, 0,
            1, 1, 1, 1, 0,
            0, 0, 0, 0, 1,
            0, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// six
            0, 0, 1, 1, 0,
            0, 1, 0, 0, 0,
            1, 0, 0, 0, 0,
            1, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// seven
            1, 1, 1, 1, 1,
            0, 0, 0, 0, 1,
            0, 0, 0, 1, 0,
            0, 0, 0, 1, 0,
            0, 0, 1, 0, 0,
            0, 0, 1, 0, 0,
            0, 1, 0, 0, 0,
        },
        {// eight
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
        {// nine
            0, 1, 1, 1, 0,
            1, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 1,
            0, 0, 0, 0, 1,
            0, 0, 0, 1, 0,
            1, 1, 1, 0, 0
        },
    };
    if (c >= 0 && c < 10) {
        render_bitmap(scaled_bitmap, num_bitmaps[c], x, y, font_width, font_height, scale, color, bg_color);
        return;
    }
    switch (c) {
    case CHAR_B: render_bitmap(scaled_bitmap, b_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_E: render_bitmap(scaled_bitmap, e_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_L: render_bitmap(scaled_bitmap, l_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_M: render_bitmap(scaled_bitmap, m_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_N: render_bitmap(scaled_bitmap, n_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_P: render_bitmap(scaled_bitmap, p_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_S: render_bitmap(scaled_bitmap, s_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_T: render_bitmap(scaled_bitmap, t_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_W: render_bitmap(scaled_bitmap, w_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    case CHAR_COLON: render_bitmap(scaled_bitmap, colon_bitmap, x, y, font_width, font_height, scale, color, bg_color); break;
    default: render_bitmap(scaled_bitmap, none_bitmap, x, y, font_width, font_height, scale, color, bg_color);
    }
}

void print_bitmap_4x6(uint16_t *scaled_bitmap, Character c, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color) {
    uint16_t none_letter[] = {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
    };
    uint16_t s_letter[] = {
        1, 1, 1, 0,
        1, 0, 0, 0,
        1, 1, 1, 0,
        0, 0, 1, 0,
        0, 0, 1, 0,
        1, 1, 1, 0,
    };
	uint16_t n_letter[] = {
        1, 1, 1, 0,
        1, 0, 1, 0,
        1, 0, 1, 0,
        1, 0, 1, 0,
        1, 0, 1, 0,
        1, 0, 1, 0,
    };
	uint16_t a_letter[] = {
        1, 1, 1, 0,
        1, 0, 1, 0,
        1, 1, 1, 0,
        1, 0, 1, 0,
        1, 0, 1, 0,
        1, 0, 1, 0,
    };
	uint16_t k_letter[] = {
        1, 0, 1, 0,
        1, 0, 1, 0,
        1, 1, 0, 0,
        1, 0, 1, 0,
        1, 0, 1, 0,
        1, 0, 1, 0,
    };
	uint16_t e_letter[] = {
        1, 1, 1, 0,
        1, 0, 0, 0,
        1, 1, 0, 0,
        1, 0, 0, 0,
        1, 0, 0, 0,
        1, 1, 1, 0,
    };
    switch (c) {
    case CHAR_S: render_bitmap(scaled_bitmap, s_letter, x, y, 4, 6, scale, color, bg_color); break;
    case CHAR_N: render_bitmap(scaled_bitmap, n_letter, x, y, 4, 6, scale, color, bg_color); break;
    case CHAR_A: render_bitmap(scaled_bitmap, a_letter, x, y, 4, 6, scale, color, bg_color); break;
    case CHAR_K: render_bitmap(scaled_bitmap, k_letter, x, y, 4, 6, scale, color, bg_color); break;
    case CHAR_E: render_bitmap(scaled_bitmap, e_letter, x, y, 4, 6, scale, color, bg_color); break;
    default: render_bitmap(scaled_bitmap, none_letter, x, y, 4, 6, scale, color, bg_color);
    }
}

void print_bitmap(uint16_t *scaled_bitmap, Character c, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color, FontSize font_size) {
    switch (font_size) {
        case FONT_SIZE_4x6: print_bitmap_4x6(scaled_bitmap, c, x, y, scale, color, bg_color); break;
        case FONT_SIZE_5x8: print_bitmap_5x8(scaled_bitmap, c, x, y, scale, color, bg_color); break;
        case FONT_SIZE_5x7: print_bitmap_5x7(scaled_bitmap, c, x, y, scale, color, bg_color); break;
    }
}

void print_bitmap_transport(uint16_t *scaled_bitmap, Transport t, bool is_ready, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color) {
    uint16_t usb_ready_bitmap[] = {
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 1, 1, 0, 1,
        1, 0, 1, 0, 1, 1, 0, 0, 1,
        1, 0, 1, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
    };
    uint16_t usb_not_ready_bitmap[] = {
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 1,
        1, 1, 0, 0, 0, 0, 0, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
    };
    uint16_t bluetooth_bitmap[] = {
        0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 1, 0, 1, 1, 0,
        0, 0, 0, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 0, 0,
        0, 0, 1, 1, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0,
    };
    uint16_t none_bitmap[] = {
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
    };
    switch (t) {
    case TRANSPORT_USB:
        if (is_ready) {
            render_bitmap(scaled_bitmap, usb_ready_bitmap, x, y, 9, 15, scale, color, bg_color);
        } else {
            render_bitmap(scaled_bitmap, usb_not_ready_bitmap, x, y, 9, 15, scale, color, bg_color);
        }
        break;
    case TRANSPORT_BLUETOOTH: render_bitmap(scaled_bitmap, bluetooth_bitmap, x, y, 9, 15, scale, color, bg_color); break;
    default: render_bitmap(scaled_bitmap, none_bitmap, x, y, 9, 15, scale, color, bg_color);
    }
}

void print_bitmap_status(uint16_t *scaled_bitmap, Status s, uint16_t x, uint16_t y, uint16_t scale, uint16_t color, uint16_t bg_color) {
        uint16_t open[] = {
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
    };
    uint16_t not_ok[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
    };
    uint16_t ok[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 1, 1, 0, 1,
        1, 0, 1, 0, 1, 1, 0, 0, 1,
        1, 0, 1, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
    };
    uint16_t none[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 1, 1, 1, 1, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 0, 0, 1, 0, 1,
        1, 0, 1, 1, 1, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
    };

    switch (s) {
    case STATUS_OPEN: render_bitmap(scaled_bitmap, open, x, y, 9, 9, scale, color, bg_color); break;
    case STATUS_OK: render_bitmap(scaled_bitmap, ok, x, y, 9, 9, scale, color, bg_color); break;
    case STATUS_NOT_OK: render_bitmap(scaled_bitmap, not_ok, x, y, 9, 9, scale, color, bg_color); break;
    default: render_bitmap(scaled_bitmap, none, x, y, 4, 6, scale, color, bg_color);
    }
}

void print_line_horizontal(uint8_t *buf_frame, uint16_t start_x, uint16_t end_x, uint16_t start_y, uint16_t end_y, uint16_t scale, uint16_t color) {
    struct display_buffer_descriptor horizontal_line_desc;

    uint16_t horizontal_line_len = end_x - start_x;

    horizontal_line_desc.buf_size = horizontal_line_len * scale;
	horizontal_line_desc.pitch = horizontal_line_len;
	horizontal_line_desc.width = horizontal_line_len;
	horizontal_line_desc.height = scale;

    fill_buffer_color(buf_frame, horizontal_line_desc.buf_size * 2u, color);
    display_write(display_dev, start_x, start_y, &horizontal_line_desc, buf_frame);
    display_write(display_dev, start_x, end_y, &horizontal_line_desc, buf_frame);
}

void print_line_vertical(uint8_t *buf_frame, uint16_t start_x, uint16_t end_x, uint16_t start_y, uint16_t end_y, uint16_t scale, uint16_t color) {
    struct display_buffer_descriptor vertical_line_desc;

    uint16_t vertical_line_len = end_y - start_y;

    vertical_line_desc.buf_size = vertical_line_len * scale;
	vertical_line_desc.pitch = scale;
	vertical_line_desc.width = scale;
	vertical_line_desc.height = vertical_line_len;

    fill_buffer_color(buf_frame, vertical_line_desc.buf_size * 2u, color);
    display_write(display_dev, start_x, start_y, &vertical_line_desc, buf_frame);
    display_write(display_dev, end_x, start_y, &vertical_line_desc, buf_frame);
}

void print_frame(uint8_t *buf_frame, uint16_t start_x, uint16_t end_x, uint16_t start_y, uint16_t end_y) {
    uint16_t scale = 1;
    print_line_horizontal(buf_frame, start_x, end_x, start_y, end_y, scale, frame_color);
    print_line_vertical(buf_frame, start_x, end_x, start_y, end_y, scale, frame_color);
}