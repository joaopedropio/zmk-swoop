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

static uint16_t status_height = 9;
static uint16_t status_width = 9;
static uint16_t status_scale = 1;

static uint16_t *scaled_bitmap_symbol;

static uint16_t symbol_scale = 2;
static uint16_t symbol_width = 9;
static uint16_t symbol_height = 15;
// static uint16_t symbol_selected_color = 0x67f7u;
// static uint16_t symbol_unselected_color = 0x5018u;
// static uint16_t symbol_bg_color = 0x08c8u;


static uint16_t *scaled_bitmap_bt_num;

static uint16_t bt_num_scale = 3;
static uint16_t bt_num_width = 5;
static uint16_t bt_num_height = 7;
// static uint16_t bt_num_unselected_color = 0x67f7u;
// static uint16_t bt_num_bg_color = 0x08c8u;

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
    print_bluetooth_profile(x, y + 0, state.active_profile_index);
}

void print_symbols(struct output_status_state state) {
    switch (state.selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        print_bitmap_transport(scaled_bitmap_symbol, TRANSPORT_USB, state.usb_is_hid_ready, 95, 283, symbol_scale, get_symbol_selected_color(), get_symbol_bg_color());
        print_bitmap_transport(scaled_bitmap_symbol, TRANSPORT_BLUETOOTH, true, 118, 283, symbol_scale, get_symbol_unselected_color(), get_symbol_bg_color());
        break;
    case ZMK_TRANSPORT_BLE:
        print_bitmap_transport(scaled_bitmap_symbol, TRANSPORT_USB, state.usb_is_hid_ready, 95, 283, symbol_scale, get_symbol_unselected_color(), get_symbol_bg_color());
        print_bitmap_transport(scaled_bitmap_symbol, TRANSPORT_BLUETOOTH, true, 118, 283, symbol_scale, get_symbol_selected_color(), get_symbol_bg_color());
        break;
    }
}

static void set_status_symbol() {
    print_bluetooth_profiles(140, 294, status_state);
    print_bluetooth_status(142, 283, status_state);
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

    widget_output_status_init();
}

void start_output_status() {
    set_status_symbol();
    status_widget_initialized = true;
}