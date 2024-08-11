/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * Based on ST7789V sample:
 * Copyright (c) 2019 Marc Reilly
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include <zmk/display.h>

#include "splash.h"

// ############## DISPLAY STATICS ##############

static const struct device *display_dev_splash;

static uint16_t snake_font_width = 4;
static uint16_t snake_font_height = 6;
static uint16_t snake_scale = 12;
static uint16_t snake_num_color = 0xFFFFu;
static uint16_t snake_bg_color = 0x004eu;

static uint8_t *buf_splash;
static struct display_buffer_descriptor buf_desc_splash;
static size_t buf_size_splash = 0;

static uint16_t *buf_splash_snake;

uint16_t swap_16_bit_color_snake(uint16_t color) {
    return (color >> 8) | (color << 8);
}

void print_bitmap_splash(uint16_t *scaled_bitmap, uint16_t bitmap[], uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t scale, uint16_t num_color, uint16_t bg_color) {
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
                    *(scaled_bitmap + index) = swap_16_bit_color_snake(color);
                    index++;
                }
            }
        }
    }
	buf_font_desc.buf_size = font_buf_size_scaled;
	buf_font_desc.pitch = font_width_scaled;
	buf_font_desc.width = font_width_scaled;
	buf_font_desc.height = font_height_scaled;
    display_write(display_dev_splash, x, y, &buf_font_desc, scaled_bitmap);
}

void render_splash_pixel(uint8_t x, uint8_t y, uint8_t offset_x, uint8_t offset_y) {
    uint16_t initial_y = (y * 31) + offset_y;
    uint16_t initial_x = (x * 60) + offset_x;
	display_write(display_dev_splash, initial_x, initial_y, &buf_desc_splash, buf_splash);
}

void print_splash(void) {
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

	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 4; y++) {
            render_splash_pixel(x, y, 0, 0);
        }
    }
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 4; y++) {
            render_splash_pixel(x, y, 0, 196);
        }
    }

	print_bitmap_splash(buf_splash_snake, s_letter, 0, 124, snake_font_width, snake_font_height, snake_scale, snake_num_color, snake_bg_color);
	print_bitmap_splash(buf_splash_snake, n_letter, 48, 124, snake_font_width, snake_font_height, snake_scale, snake_num_color, snake_bg_color);
	print_bitmap_splash(buf_splash_snake, a_letter, 96, 124, snake_font_width, snake_font_height, snake_scale, snake_num_color, snake_bg_color);
	print_bitmap_splash(buf_splash_snake, k_letter, 144, 124, snake_font_width, snake_font_height, snake_scale, snake_num_color, snake_bg_color);
	print_bitmap_splash(buf_splash_snake, e_letter, 192, 124, snake_font_width, snake_font_height, snake_scale, snake_num_color, snake_bg_color);
}

// ############## Display setup ################

static void fill_buffer_splash(uint8_t *buf, size_t buf_size, uint32_t color) {
	for (size_t idx = 0; idx < buf_size; idx += 2) {
		*(buf + idx + 0) = (color >> 8) & 0xFFu;
		*(buf + idx + 1) = (color >> 0) & 0xFFu;
	}
}

void buffer_splash_snake_init() {
	buf_splash_snake = k_malloc((snake_font_width * snake_scale) * (snake_font_height * snake_scale) * 2u);
}

void buffer_splash_init() {
	buf_size_splash = 60 * 31 * 2u;
	buf_splash = k_malloc(buf_size_splash);
	buf_desc_splash.pitch = 60;
	buf_desc_splash.width = 60;
	buf_desc_splash.height = 31;
	fill_buffer_splash(buf_splash, buf_size_splash, 0x004eu);
}

void display_setup_splash(void) {
	display_dev_splash = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev_splash)) {
		LOG_ERR("Device %s not found. Aborting sample.", display_dev_splash->name);
		return;
	}
	buffer_splash_snake_init();
	buffer_splash_init();
}

void zmk_widget_splash_init() {
	display_setup_splash();
}
