/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/usb.h>
#include <zmk/ble.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#include "battery_status.h"
#include "helpers/display.h"

static bool battery_widget_initialized = false;
static struct peripheral_battery_state battery_state;
static uint16_t *scaled_bitmap_1;

static const uint16_t scale = 4;
static const uint16_t font_width = 5;
static const uint16_t font_height = 8;

static const uint16_t start_x_peripheral_1 = 10;
static const uint16_t start_x_peripheral_2 = 170;
static const uint16_t start_y = 282;

struct peripheral_battery_state {
    uint8_t source;
    uint8_t level;
};

void print_percentage(uint8_t digit, uint16_t x, uint16_t y, uint16_t scale, uint16_t num_color, uint16_t bg_color, uint16_t percentage_color) {
    if (digit == 0) {
        print_bitmap(scaled_bitmap_1, CHAR_DASH, x + 0, y, scale, num_color, bg_color, FONT_SIZE_5x8);
        print_bitmap(scaled_bitmap_1, CHAR_DASH, x + 22, y, scale, num_color, bg_color, FONT_SIZE_5x8);
        print_bitmap(scaled_bitmap_1, CHAR_PERCENTAGE, x + 44, y, scale, percentage_color, bg_color, FONT_SIZE_5x8);
        return;
    }

    if (digit > 99) {
        print_bitmap(scaled_bitmap_1, CHAR_F, x + 0,  y, scale, num_color, bg_color, FONT_SIZE_5x8);
        print_bitmap(scaled_bitmap_1, CHAR_U, x + 22, y, scale, num_color, bg_color, FONT_SIZE_5x8);
        print_bitmap(scaled_bitmap_1, CHAR_L, x + 44, y, scale, num_color, bg_color, FONT_SIZE_5x8);
        return;
    }

    uint16_t first_num = digit / 10;
    uint16_t second_num = digit % 10;

    print_bitmap(scaled_bitmap_1, first_num, x + 0, y, scale, num_color, bg_color, FONT_SIZE_5x8);
    print_bitmap(scaled_bitmap_1, second_num, x + 22, y, scale, num_color, bg_color, FONT_SIZE_5x8);
    print_bitmap(scaled_bitmap_1, CHAR_PERCENTAGE, x + 44, y, scale, percentage_color, bg_color, FONT_SIZE_5x8);
}

static void set_battery_symbol() {
    if (battery_state.source == 0) {
        print_percentage(battery_state.level, start_x_peripheral_1, start_y, scale, get_battery_num_color(), get_battery_bg_color(), get_battery_percentage_color());
    } else {
        print_percentage(battery_state.level, start_x_peripheral_2, start_y, scale, get_battery_num_color(), get_battery_bg_color(), get_battery_percentage_color());
    }
}

void battery_status_update_cb(struct peripheral_battery_state state) {
    battery_state = state;
    if (battery_widget_initialized) {
        set_battery_symbol();
    }
}

static struct peripheral_battery_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *ev = as_zmk_peripheral_battery_state_changed(eh);
    return (struct peripheral_battery_state){
        .source = ev->source,
        .level = ev->state_of_charge,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct peripheral_battery_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_peripheral_battery_state_changed);

void print_empty_batteries() {
    battery_state.source = 0;
    battery_state.level = 0;
    set_battery_symbol();
    battery_state.source = 1;
    battery_state.level = 0;
    set_battery_symbol();
}

void zmk_widget_peripheral_battery_status_init() {
    uint16_t bitmap_size = (font_width * scale) * (font_height * scale);

    scaled_bitmap_1 = k_malloc(bitmap_size * 2 * sizeof(uint16_t));
    
    widget_battery_status_init();
}

void start_battery_status() {
    print_empty_batteries();
    battery_widget_initialized = true;
}