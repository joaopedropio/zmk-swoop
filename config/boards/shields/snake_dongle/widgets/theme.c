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
#include "helpers/display.h"

#define COLORS_PER_THEME 6

static uint8_t themes_colors_len = 7;
static uint32_t themes_colors[][COLORS_PER_THEME] = {
    {0x222323u, 0xff4adcu, 0x3dff98u, 0xf0f6f0u, 0x000000u, 0xddddddu},
    {0x302387u, 0xff3796u, 0x00faacu, 0xfffdafu, 0x000000u, 0xddddddu},
    {0x332c50u, 0x46878fu, 0x94e344u, 0xe2f3e4u, 0x000000u, 0xddddddu},
    {0x0f052du, 0x203671u, 0x36868fu, 0x5fc75du, 0x000000u, 0xddddddu},
    {0xfcdeeau, 0xff4d6du, 0x265935u, 0x012824u, 0x000000u, 0xddddddu},
    {0x282328u, 0x545c7eu, 0xc56981u, 0xa3a29au, 0x000000u, 0xddddddu},
    {0xfce4a8u, 0x71969fu, 0xd71a21u, 0x01334eu, 0x000000u, 0xddddddu},
};

static uint8_t current_theme = 0;

static uint16_t theme_font_scale = 2;
static uint16_t theme_font_width = 5;
static uint16_t theme_font_height = 7;
static uint16_t *scaled_bitmap_theme_font;

static uint16_t theme_x = 140;
static uint16_t theme_y = 20;

static Character int_to_num_char(uint8_t i) {
    switch (i) {
        case 0: return CHAR_0;
        case 1: return CHAR_1;
        case 2: return CHAR_2;
        case 3: return CHAR_3;
        case 4: return CHAR_4;
        case 5: return CHAR_5;
        case 6: return CHAR_6;
        case 7: return CHAR_7;
        case 8: return CHAR_8;
        case 9: return CHAR_9;
    }
    return CHAR_NONE;
}

void print_themes() {
    Character theme_template[] = {
        CHAR_T,
        CHAR_H,
        CHAR_E,
        CHAR_M,
        CHAR_E,
        CHAR_EMPTY,
        CHAR_NONE
    };

    uint16_t line_gap_pixels = 2;
    uint16_t char_gap_pixels = 1;
    uint8_t theme_template_len = 7;
    uint16_t theme_font_height_scaled = theme_font_height * theme_font_scale;

    for (uint8_t i = 0; i < themes_colors_len; i++) {
        uint16_t actual_y = theme_y + ((line_gap_pixels + theme_font_height_scaled) * i);
        theme_template[6] = int_to_num_char(i);
        if (i == current_theme) {
            print_string(scaled_bitmap_theme_font, theme_template, theme_x, actual_y, theme_font_scale, get_symbol_unselected_color(), get_symbol_bg_color(), FONT_SIZE_5x7, char_gap_pixels, theme_template_len);
        } else {
            print_string(scaled_bitmap_theme_font, theme_template, theme_x, actual_y, theme_font_scale, get_symbol_selected_color(), get_symbol_bg_color(), FONT_SIZE_5x7, char_gap_pixels, theme_template_len);
        }
    }
}

void set_next_theme() {
    set_next_theme_number();
    set_colors(
        themes_colors[current_theme][0],
        themes_colors[current_theme][1],
        themes_colors[current_theme][2],
        themes_colors[current_theme][3],
        themes_colors[current_theme][4],
        themes_colors[current_theme][5]
    );



    // if (current_theme == 2) {
    //     set_colors(0x222323u, 0xff4adcu, 0x3dff98u, 0xf0f6f0u, 0x000000u, 0xddddddu);
    //     current_theme = 1;
    // } else if (current_theme == 1) {
    //     set_colors(0x302387u, 0xff3796u, 0x00faacu, 0xfffdafu, 0x000000u, 0xddddddu);
    //     current_theme = 2;
    // }
}

void set_next_theme_number() {
    current_theme++;
    if (current_theme >= themes_colors_len) {
        current_theme = 0;
    }
}

void theme_init() {
    uint16_t theme_font_size = (theme_font_width * theme_font_scale) * (theme_font_height * theme_font_scale);

    scaled_bitmap_theme_font = k_malloc(theme_font_size * 2 * sizeof(uint16_t));
}