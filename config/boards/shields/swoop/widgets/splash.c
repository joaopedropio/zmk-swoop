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

static uint8_t *buf_splash;
static struct display_buffer_descriptor buf_desc_splash;
static size_t buf_size_splash = 0;

static uint16_t *buf_splash_snake;

void render_splash_pixel(uint8_t x, uint8_t y, uint8_t offset_x, uint8_t offset_y) {
    uint16_t initial_y = (y * 31) + offset_y;
    uint16_t initial_x = (x * 60) + offset_x;
	display_write_wrapper(initial_x, initial_y, &buf_desc_splash, buf_splash);
}

void print_background(void) {
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 4; y++) {
            render_splash_pixel(x, y, 0, 0);
        }
    }
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 4; y++) {
            render_splash_pixel(x, y, 0, 72);
        }
    }
	for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 4; y++) {
            render_splash_pixel(x, y, 0, 196);
        }
    }
}

void print_splash(void) {
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

	print_bitmap(buf_splash_snake, CHAR_S, 0, 124, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
	print_bitmap(buf_splash_snake, CHAR_N, 48, 124, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
	print_bitmap(buf_splash_snake, CHAR_A, 96, 124, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
	print_bitmap(buf_splash_snake, CHAR_K, 144, 124, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
	print_bitmap(buf_splash_snake, CHAR_E, 192, 124, snake_scale, get_splash_num_color(), get_splash_bg_color(), FONT_SIZE_4x6);
}

// ############## Display setup ################

void buffer_splash_snake_init() {
	buf_splash_snake = k_malloc((snake_font_width * snake_scale) * (snake_font_height * snake_scale) * 2u);
}

void buffer_splash_init() {
	buf_size_splash = 60 * 31 * 2u;
	buf_splash = k_malloc(buf_size_splash);
	buf_desc_splash.pitch = 60;
	buf_desc_splash.width = 60;
	buf_desc_splash.height = 31;
	fill_buffer_color(buf_splash, buf_size_splash, get_splash_bg_color());
}

void zmk_widget_splash_init() {
	buffer_splash_snake_init();
	buffer_splash_init();
}

void clean_up_splash() {
    k_free(buf_splash);
    k_free(buf_splash_snake);
}