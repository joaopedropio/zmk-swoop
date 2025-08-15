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
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>

#include "output_status.h"
#include "helpers/display.h"

static bool status_widget_initialized = false;
static struct output_status_state status_state;
static uint16_t *scaled_bitmap_status;
static uint16_t *scaled_bitmap_symbol;
static uint16_t *scaled_bitmap_bt_num;

static const uint16_t status_height = 9;
static const uint16_t status_width = 9;
static const uint16_t status_scale = 5;

static const uint16_t symbol_scale = 5;
static const uint16_t symbol_width = 9;
static const uint16_t symbol_height = 15;

static const uint16_t bt_num_scale = 7;
static const uint16_t bt_num_width = 5;
static const uint16_t bt_num_height = 7;

static const uint16_t bluetooth_profiles_x = 14;
static const uint16_t bluetooth_profiles_y = 20;
static const uint16_t bluetooth_status_x = 68;
static const uint16_t bluetooth_status_y = 20;
static const uint16_t usb_symbol_x = 12;
static const uint16_t ble_symbol_x = 72;
static const uint16_t output_symbols_y = 80;


struct output_status_state {
    struct zmk_endpoint_instance selected_endpoint;
    int active_profile_index;
    bool active_profile_connected;
    bool active_profile_bonded;
    bool usb_is_hid_ready;
};

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
            render_bitmap(scaled_bitmap, usb_ready_bitmap, x, y, symbol_width, symbol_height, scale, color, bg_color);
        } else {
            render_bitmap(scaled_bitmap, usb_not_ready_bitmap, x, y, symbol_width, symbol_height, scale, color, bg_color);
        }
        break;
    case TRANSPORT_BLUETOOTH: render_bitmap(scaled_bitmap, bluetooth_bitmap, x, y, symbol_width, symbol_height, scale, color, bg_color); break;
    default: render_bitmap(scaled_bitmap, none_bitmap, x, y, symbol_width, symbol_height, scale, color, bg_color);
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
    case STATUS_OPEN: render_bitmap(scaled_bitmap, open, x, y, status_width, status_height, scale, color, bg_color); break;
    case STATUS_OK: render_bitmap(scaled_bitmap, ok, x, y, status_width, status_height, scale, color, bg_color); break;
    case STATUS_NOT_OK: render_bitmap(scaled_bitmap, not_ok, x, y, status_width, status_height, scale, color, bg_color); break;
    default: render_bitmap(scaled_bitmap, none, x, y, 4, 6, scale, color, bg_color);
    }
}

static struct output_status_state get_state(const zmk_event_t *_eh) {
    return (struct output_status_state){
        .selected_endpoint = zmk_endpoints_selected(),
        .active_profile_index = zmk_ble_active_profile_index(),
        .active_profile_connected = zmk_ble_active_profile_is_connected(),
        .active_profile_bonded = !zmk_ble_active_profile_is_open(),
        .usb_is_hid_ready = zmk_usb_is_hid_ready()
    };
}

void print_bluetooth_status(uint16_t x, uint16_t y, struct output_status_state state) {
    if (state.active_profile_bonded) {
        if (state.active_profile_connected) {
            print_bitmap_status(scaled_bitmap_status, STATUS_OK, x, y, status_scale, get_bt_num_color(), get_bt_bg_color());
        } else {
            print_bitmap_status(scaled_bitmap_status, STATUS_NOT_OK, x, y, status_scale, get_bt_num_color(), get_bt_bg_color());
        }
    } else {
        print_bitmap_status(scaled_bitmap_status, STATUS_OPEN, x, y, status_scale, get_bt_num_color(), get_bt_bg_color());
    }
}

void print_bluetooth_profile(uint16_t x, uint16_t y, int active_profile) {
    if (active_profile < 0 || active_profile > 4) {
        print_bitmap(scaled_bitmap_bt_num, CHAR_NONE, x, y, bt_num_scale, get_bt_num_color(), get_bt_bg_color(), FONT_SIZE_5x7);
        return;
    }
    print_bitmap(scaled_bitmap_bt_num, active_profile + 1, x, y, bt_num_scale, get_bt_num_color(), get_bt_bg_color(), FONT_SIZE_5x7);
}

void print_bluetooth_profiles(uint16_t x, uint16_t y, struct output_status_state state) {
    print_bluetooth_profile(x, y, state.active_profile_index);
}

void print_symbols(uint16_t usb_x, uint16_t ble_x, uint16_t y, struct output_status_state state) {
    switch (state.selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        print_bitmap_transport(scaled_bitmap_symbol, TRANSPORT_USB, state.usb_is_hid_ready, usb_x, y, symbol_scale, get_symbol_selected_color(), get_symbol_bg_color());
        print_bitmap_transport(scaled_bitmap_symbol, TRANSPORT_BLUETOOTH, true, ble_x, y, symbol_scale, get_symbol_unselected_color(), get_symbol_bg_color());
        break;
    case ZMK_TRANSPORT_BLE:
        print_bitmap_transport(scaled_bitmap_symbol, TRANSPORT_USB, state.usb_is_hid_ready, usb_x, y, symbol_scale, get_symbol_unselected_color(), get_symbol_bg_color());
        print_bitmap_transport(scaled_bitmap_symbol, TRANSPORT_BLUETOOTH, true, ble_x, y, symbol_scale, get_symbol_selected_color(), get_symbol_bg_color());
        break;
    }
}

void set_status_symbol() {
    print_bluetooth_profiles(bluetooth_profiles_x, bluetooth_profiles_y, status_state);
    print_bluetooth_status(bluetooth_status_x, bluetooth_status_y, status_state);
    print_symbols(usb_symbol_x, ble_symbol_x, output_symbols_y, status_state);
}

void output_status_update_cb(struct output_status_state state) {
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

    widget_output_status_init();
}

void start_output_status() {
    set_status_symbol();
    status_widget_initialized = true;
}

void stop_output_status(void) {
    status_widget_initialized = false;
}