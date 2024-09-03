#include <zephyr/drivers/display.h>
#include "font.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const struct device *display_dev;

struct device* get_display() {
    return display_dev;
}

void init_display(void) {
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device %s not found. Aborting sample.", display_dev->name);
		return;
	}
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
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
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