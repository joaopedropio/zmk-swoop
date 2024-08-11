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

static const struct device *display_dev_status;

static uint16_t *scaled_bitmap_symbol;

static uint16_t symbol_scale = 2;
static uint16_t symbol_width = 8;
static uint16_t symbol_height = 13;
static uint16_t symbol_selected_color = 0x001Fu;
static uint16_t symbol_unselected_color = 0x0000u;
static uint16_t symbol_bg_color = 0xFFFFu;

// #############   ####################

// green = 0x001Fu
// red   = 0x07E0u


static uint16_t *scaled_bitmap_bt_num;

static uint16_t bt_num_scale = 1;
static uint16_t bt_num_width = 5;
static uint16_t bt_num_height = 8;
static uint16_t bt_num_selected_color = 0x001Fu;
static uint16_t bt_num_unselected_color = 0x0000u;
static uint16_t bt_num_bg_color = 0xFFFFu;

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

LV_IMG_DECLARE(sym_usb);
LV_IMG_DECLARE(sym_bt);
LV_IMG_DECLARE(sym_ok);
LV_IMG_DECLARE(sym_nok);
LV_IMG_DECLARE(sym_open);
LV_IMG_DECLARE(sym_1);
LV_IMG_DECLARE(sym_2);
LV_IMG_DECLARE(sym_3);
LV_IMG_DECLARE(sym_4);
LV_IMG_DECLARE(sym_5);

const lv_img_dsc_t *sym_num[] = {
    &sym_1,
    &sym_2,
    &sym_3,
    &sym_4,
    &sym_5,
};

// enum output_symbol {
//     output_symbol_usb,
//     output_symbol_usb_hid_status,
//     output_symbol_bt,
//     output_symbol_bt_number,
//     output_symbol_bt_status,
//     output_symbol_selection_line
// };

// enum selection_line_state {
//     selection_line_state_usb,
//     selection_line_state_bt
// } current_selection_line_state;

// lv_point_t selection_line_points[] = { {-1, 0}, {12, 0} }; // will be replaced with  

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
                    *(scaled_bitmap + index) = color;
                    index++;
                }
            }
        }
    }
	buf_desc.buf_size = buf_size_scaled;
	buf_desc.pitch = width_scaled;
	buf_desc.width = width_scaled;
	buf_desc.height = height_scaled;
    display_write(display_dev_status, x, y, &buf_desc, scaled_bitmap);
}

void print_bluetooth_profile(uint16_t x, uint16_t y, int profile, int active_profile) {
        uint16_t num_bitmaps[5][40] = {
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
    };

    if (profile == active_profile) {
        print_bitmap_symbol(scaled_bitmap_bt_num, num_bitmaps[profile], x, y, bt_num_width, bt_num_height, bt_num_scale, bt_num_selected_color, bt_num_bg_color);
    } else {
        print_bitmap_symbol(scaled_bitmap_bt_num, num_bitmaps[profile], x, y, bt_num_width, bt_num_height, bt_num_scale, bt_num_unselected_color, bt_num_bg_color);
    }
}

void print_bluetooth_profiles(uint16_t x, uint16_t y, struct output_status_state state) {
    print_bluetooth_profile(x, y + 0 , 0, state.active_profile_index);
    print_bluetooth_profile(x, y + 10, 1, state.active_profile_index);
    print_bluetooth_profile(x, y + 20, 2, state.active_profile_index);
    print_bluetooth_profile(x, y + 30, 3, state.active_profile_index);
    print_bluetooth_profile(x, y + 40, 4, state.active_profile_index);
}

void print_symbols(struct output_status_state state) {
    uint16_t usb_bitmap[] = {
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 0, 1, 1, 0, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
    };
    uint16_t bluetooth_bitmap[] = {
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        1, 0, 0, 1, 0, 1, 1, 0,
        1, 1, 0, 1, 0, 0, 1, 0,
        0, 1, 1, 1, 0, 1, 1, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 1, 1, 1, 0, 1, 1, 0,
        1, 1, 0, 1, 0, 0, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
    };

    switch (state.selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        print_bitmap_symbol(scaled_bitmap_symbol, usb_bitmap, 110, 265, symbol_width, symbol_height, symbol_scale, symbol_selected_color, symbol_bg_color);
        print_bitmap_symbol(scaled_bitmap_symbol, bluetooth_bitmap, 110, 295, symbol_width, symbol_height, symbol_scale, symbol_unselected_color, symbol_bg_color);
        break;
    case ZMK_TRANSPORT_BLE:
        print_bitmap_symbol(scaled_bitmap_symbol, usb_bitmap, 110, 265, symbol_width, symbol_height, symbol_scale, symbol_unselected_color, symbol_bg_color);
        print_bitmap_symbol(scaled_bitmap_symbol, bluetooth_bitmap, 110, 295, symbol_width, symbol_height, symbol_scale, symbol_selected_color, symbol_bg_color);
        break;
    }
}

static void set_status_symbol(lv_obj_t *widget, struct output_status_state state) {

    print_bluetooth_profiles(130, 268, state);

    print_symbols(state);

    // if (state.usb_is_hid_ready) {
    //     lv_img_set_src(usb_hid_status, &sym_ok);
    // } else {
    //     lv_img_set_src(usb_hid_status, &sym_nok);
    // }

    // if (state.active_profile_index < (sizeof(sym_num) / sizeof(lv_img_dsc_t *))) {
    //     lv_img_set_src(bt_number, sym_num[state.active_profile_index]);
    // } else {
    //     lv_img_set_src(bt_number, &sym_nok);
    // }
    
    // if (state.active_profile_bonded) {
    //     if (state.active_profile_connected) {
    //         lv_img_set_src(bt_status, &sym_ok);
    //     } else {
    //         lv_img_set_src(bt_status, &sym_nok);
    //     }
    // } else {
    //     lv_img_set_src(bt_status, &sym_open);
    // }
}

static void output_status_update_cb(struct output_status_state state) {
    struct zmk_widget_output_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_status_symbol(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_conn_state_changed);

void display_setup_status(void) {
	display_dev_status = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev_status)) {
		LOG_ERR("Device %s not found. Aborting sample.", display_dev_status->name);
		return;
	}
}

int zmk_widget_output_status_init(struct zmk_widget_output_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    
    lv_obj_set_size(widget->obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    // lv_obj_t *usb = lv_img_create(widget->obj);
    // lv_obj_align(usb, LV_ALIGN_TOP_LEFT, 1, 4);
    // lv_img_set_src(usb, &sym_usb);

    // lv_obj_t *usb_hid_status = lv_img_create(widget->obj);
    // lv_obj_align_to(usb_hid_status, usb, LV_ALIGN_BOTTOM_LEFT, 2, -7);

    // lv_obj_t *bt = lv_img_create(widget->obj);
    // lv_obj_align_to(bt, usb, LV_ALIGN_OUT_RIGHT_TOP, 6, 0);
    // lv_img_set_src(bt, &sym_bt);

    // lv_obj_t *bt_number = lv_img_create(widget->obj);
    // lv_obj_align_to(bt_number, bt, LV_ALIGN_OUT_RIGHT_TOP, 2, 7);

    // lv_obj_t *bt_status = lv_img_create(widget->obj);
    // lv_obj_align_to(bt_status, bt, LV_ALIGN_OUT_RIGHT_TOP, 2, 1);
    
    // static lv_style_t style_line;
    // lv_style_init(&style_line);
    // lv_style_set_line_width(&style_line, 2);

    // lv_obj_t *selection_line;
    // selection_line = lv_line_create(widget->obj);
    // lv_line_set_points(selection_line, selection_line_points, 2);
    // lv_obj_add_style(selection_line, &style_line, 0);
    // lv_obj_align_to(selection_line, usb, LV_ALIGN_OUT_TOP_LEFT, 3, -1);

    display_setup_status();

    uint16_t bitmap_size_symbol = (symbol_width * symbol_scale) * (symbol_height * symbol_scale);

    scaled_bitmap_symbol = k_malloc(bitmap_size_symbol * 2 * sizeof(uint16_t));

    uint16_t bitmap_size_bt_num = (bt_num_width * bt_num_scale) * (bt_num_height * bt_num_scale);

    scaled_bitmap_bt_num = k_malloc(bitmap_size_bt_num * 2 * sizeof(uint16_t));
 
    sys_slist_append(&widgets, &widget->node);

    widget_output_status_init();
    return 0;
}

lv_obj_t *zmk_widget_output_status_obj(struct zmk_widget_output_status *widget) {
    return widget->obj;
}
