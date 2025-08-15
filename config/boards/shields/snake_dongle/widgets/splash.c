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
#include "helpers/display.h"

// ############## DISPLAY STATICS ##############

static uint16_t snake_font_width = 4;
static uint16_t snake_font_height = 6;
static uint16_t snake_scale = 12;
static uint16_t *buf_splash_snake;

static uint16_t snake_logo_start_height = 84;
static uint16_t background_pixel_width = 60;
static uint16_t background_pixel_height = 42;

static uint8_t *buf_background_splash;
static struct display_buffer_descriptor buf_desc_background_splash;
static size_t buf_size_background_splash = 0;

void render_background_splash_pixel(uint8_t x, uint8_t y, uint8_t offset_x, uint8_t offset_y) {
    uint16_t initial_y = (y * background_pixel_height) + offset_y;
    uint16_t initial_x = (x * background_pixel_width) + offset_x;
	display_write_wrapper(initial_x, initial_y, &buf_desc_background_splash, buf_background_splash);
}

void print_background(void) {
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 2; y++) {
            render_background_splash_pixel(x, y, 0, 0);
        }
    }
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 2; y++) {
            render_background_splash_pixel(x, y, 0, 84);
        }
    }
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 2; y++) {
            render_background_splash_pixel(x, y, 0, 168);
        }
    }
}

void print_splash(void) {
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 2; y++) {
            render_background_splash_pixel(x, y, 0, 0);
        }
    }
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 2; y++) {
            render_background_splash_pixel(x, y, 0, 156);
        }
    }

	print_bitmap(buf_splash_snake, CHAR_S, 0, snake_logo_start_height, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
	print_bitmap(buf_splash_snake, CHAR_N, 48, snake_logo_start_height, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
	print_bitmap(buf_splash_snake, CHAR_A, 96, snake_logo_start_height, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
	print_bitmap(buf_splash_snake, CHAR_K, 144, snake_logo_start_height, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
	print_bitmap(buf_splash_snake, CHAR_E, 192, snake_logo_start_height, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
}

// ############## Display setup ################

void buffer_splash_snake_init() {
	buf_splash_snake = k_malloc((snake_font_width * snake_scale) * (snake_font_height * snake_scale) * 2u);
}

void buffer_background_splash_init() {
	buf_size_background_splash = background_pixel_width * background_pixel_height * 2u;
	buf_background_splash = k_malloc(buf_size_background_splash);
	buf_desc_background_splash.pitch = background_pixel_width;
	buf_desc_background_splash.width = background_pixel_width;
	buf_desc_background_splash.height = background_pixel_height;
	fill_buffer_color(buf_background_splash, buf_size_background_splash, get_splash_bg_color());
}

void zmk_widget_splash_init() {
	buffer_splash_snake_init();
	buffer_background_splash_init();
}

void clean_up_splash() {
    k_free(buf_background_splash);
    k_free(buf_splash_snake);
}