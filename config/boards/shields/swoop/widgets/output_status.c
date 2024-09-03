/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>

#include "output_status.h"
#include "helpers/font.h"

static bool status_widget_initialized = false;
static struct output_status_state status_state;

static uint16_t *scaled_bitmap_status;

static uint16_t status_height = 9;
static uint16_t status_width = 9;
static uint16_t status_scale = 1;

static uint16_t *scaled_bitmap_symbol;

static uint16_t symbol_scale = 2;
static uint16_t symbol_width = 9;
static uint16_t symbol_height = 15;
static uint16_t symbol_selected_color = 0x67f7u;
static uint16_t symbol_unselected_color = 0xFFFFu;
static uint16_t symbol_bg_color = 0x004eu;

// #############   ####################

// green = 0x001Fu
// red   = 0x07E0u


static uint16_t *scaled_bitmap_bt_num;

static uint16_t bt_num_scale = 3;
static uint16_t bt_num_width = 5;
static uint16_t bt_num_height = 7;
static uint16_t bt_num_unselected_color = 0xFFFFu;
static uint16_t bt_num_bg_color = 0x004eu;

struct output_status_state {
    struct zmk_endpoint_instance selected_endpoint;
    int active_profile_index;
    bool active_profile_connected;
    bool active_profile_bonded;
    bool usb_is_hid_ready;
};

static struct output_status_state get_state(const zmk_event_t *_eh) {
    return (struct output_status_state){
        .selected_endpoint = zmk_endpoints_selected(),
        .active_profile_index = zmk_ble_active_profile_index(),
        .active_profile_connected = zmk_ble_active_profile_is_connected(),
        .active_profile_bonded = !zmk_ble_active_profile_is_open(),
        .usb_is_hid_ready = zmk_usb_is_hid_ready()
    };
}

uint16_t swap_16_bit_color_symbol(uint16_t color) {
    return (color >> 8) | (color << 8);
}

void print_bitmap_symbol(uint16_t *scaled_bitmap, uint16_t bitmap[], uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t scale, uint16_t num_color, uint16_t bg_color) {
	struct display_buffer_descriptor buf_desc;

    uint16_t color;
    uint16_t pixel;
    uint16_t width_scaled = width * scale;
    uint16_t height_scaled = height * scale;
    uint16_t buf_size_scaled = width_scaled * height_scaled;
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
                    *(scaled_bitmap + index) = swap_16_bit_color_symbol(color);
                    index++;
                }
            }
        }
    }
	buf_desc.buf_size = buf_size_scaled;
	buf_desc.pitch = width_scaled;
	buf_desc.width = width_scaled;
	buf_desc.height = height_scaled;
    display_write(get_display(), x, y, &buf_desc, scaled_bitmap);
}

void print_bluetooth_status(uint16_t x, uint16_t y, struct output_status_state state) {
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

    if (state.active_profile_bonded) {
        if (state.active_profile_connected) {
            print_bitmap_symbol(scaled_bitmap_status, ok, x, y, status_width, status_height, status_scale, bt_num_unselected_color, bt_num_bg_color);
        } else {
            print_bitmap_symbol(scaled_bitmap_status, not_ok, x, y, status_width, status_height, status_scale, bt_num_unselected_color, bt_num_bg_color);
        }
    } else {
        print_bitmap_symbol(scaled_bitmap_status, open, x, y, status_width, status_height, status_scale, bt_num_unselected_color, bt_num_bg_color);
    }
}

void print_bluetooth_profile(uint16_t x, uint16_t y, int active_profile) {
    uint16_t num_bitmaps[5][35] = {
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
            1, 1, 1, 1, 1,
            0, 0, 0, 0, 1,
            1, 0, 0, 0, 1,
            0, 1, 1, 1, 0
        },
    };
    uint16_t none[] = {
        0, 1, 1, 1, 0,
        1, 0, 0, 0, 1,
        0, 0, 0, 0, 1,
        0, 0, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
    };

    if (active_profile < 1 || active_profile > 5) {
        print_bitmap_symbol(scaled_bitmap_bt_num, none, x, y, bt_num_width, bt_num_height, bt_num_scale, bt_num_unselected_color, bt_num_bg_color);
    }

    print_bitmap_symbol(scaled_bitmap_bt_num, num_bitmaps[active_profile], x, y, bt_num_width, bt_num_height, bt_num_scale, bt_num_unselected_color, bt_num_bg_color);
}

void print_bluetooth_profiles(uint16_t x, uint16_t y, struct output_status_state state) {
    print_bluetooth_profile(x, y + 0, state.active_profile_index);
}

void print_symbols(struct output_status_state state) {
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

    switch (state.selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        if (state.usb_is_hid_ready) {
            print_bitmap_symbol(scaled_bitmap_symbol, usb_ready_bitmap, 95, 280, symbol_width, symbol_height, symbol_scale, symbol_selected_color, symbol_bg_color);
        } else {
            print_bitmap_symbol(scaled_bitmap_symbol, usb_not_ready_bitmap, 95, 280, symbol_width, symbol_height, symbol_scale, symbol_selected_color, symbol_bg_color);
        }
        print_bitmap_symbol(scaled_bitmap_symbol, bluetooth_bitmap, 115, 280, symbol_width, symbol_height, symbol_scale, symbol_unselected_color, symbol_bg_color);
        break;
    case ZMK_TRANSPORT_BLE:
        if (state.usb_is_hid_ready) {
            print_bitmap_symbol(scaled_bitmap_symbol, usb_ready_bitmap, 95, 280, symbol_width, symbol_height, symbol_scale, symbol_unselected_color, symbol_bg_color);
        } else {
            print_bitmap_symbol(scaled_bitmap_symbol, usb_not_ready_bitmap, 95, 280, symbol_width, symbol_height, symbol_scale, symbol_unselected_color, symbol_bg_color);
        }
        print_bitmap_symbol(scaled_bitmap_symbol, bluetooth_bitmap, 115, 280, symbol_width, symbol_height, symbol_scale, symbol_selected_color, symbol_bg_color);
        break;
    }
}

static void set_status_symbol() {
    print_bluetooth_profiles(133, 290, status_state);
    print_bluetooth_status(135, 280, status_state);
    print_symbols(status_state);
}

static void output_status_update_cb(struct output_status_state state) {
    status_state = state;
    if (status_widget_initialized) {
        set_status_symbol();
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_conn_state_changed);


void zmk_widget_output_status_init() {
    uint16_t bitmap_size_symbol = (symbol_width * symbol_scale) * (symbol_height * symbol_scale);

    scaled_bitmap_symbol = k_malloc(bitmap_size_symbol * 2 * sizeof(uint16_t));

    uint16_t bitmap_size_bt_num = (bt_num_width * bt_num_scale) * (bt_num_height * bt_num_scale);

    scaled_bitmap_bt_num = k_malloc(bitmap_size_bt_num * 2 * sizeof(uint16_t));

    uint16_t bitmap_size_status = (status_width * status_scale) * (status_height * status_scale);

    scaled_bitmap_status = k_malloc(bitmap_size_status * 2 * sizeof(uint16_t));

    status_widget_initialized = true;
    widget_output_status_init();
}