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
#include <zmk/display/widgets/layer_status.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

#include <zmk/display.h>

#include "layer_switch.h"
#include "snake.h"
#include "output_status.h"
#include "battery_status.h"
#include "helpers/display.h"
#include "theme.h"

static bool layer_switch_initialized = false;
static struct layer_status_state ls_state;

// static bool layer_switch_initialized = false;
// static struct zmk_keycode_state_changed kc_state;

struct layer_status_state {
    uint8_t index;
    const char *label;
};

// struct zmk_keycode_state_changed {
//     uint16_t usage_page;
//     uint32_t keycode;
//     uint8_t implicit_modifiers;
//     uint8_t explicit_modifiers;
//     bool state;
//     int64_t timestamp;
// };

    // uint32_t color1 = 0x302387u;
    // uint32_t color2 = 0xff3796u;
    // uint32_t color3 = 0x00faacu;
    // uint32_t color4 = 0xfffdafu;

void set_layer_symbol() {
    if (ls_state.index == 2) {
        set_next_theme();
        set_layer(ls_state.index, 1);
    } else if (ls_state.index == 1) {
        stop_snake();
        clear_area();
        start_battery_status();
        start_output_status();
        set_status_symbol();
        set_battery_symbol();
        print_themes();
    } else {
        stop_output_status();
        stop_battery_status();
        restart_snake();
    }
}

void set_layer(uint8_t current_layer, uint8_t target_layer) {
    zmk_keymap_layer_deactivate(current_layer);
    zmk_keymap_layer_activate(target_layer);
    // maybe use this? 
    // int zmk_keymap_layer_to(zmk_keymap_layer_id_t layer);
}

static void layer_status_update_cb(struct layer_status_state state) {
    ls_state = state;
    if (layer_switch_initialized) {
        set_layer_symbol();
    }
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct layer_status_state) {
        .index = index,
        .label = zmk_keymap_layer_name(index)
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

void zmk_widget_layer_switch_init() {
	widget_layer_status_init();
}


void start_layer_switch() {
    layer_switch_initialized = true;
}
