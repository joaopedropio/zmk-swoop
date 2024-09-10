/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "custom_status_screen.h"
#include "widgets/battery_status.h"
#include "widgets/output_status.h"
#include "widgets/splash.h"
#include "widgets/snake.h"
#include "widgets/helpers/display.h"
#include "widgets/frame.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const uint8_t SPLASH_DURATION = 50;
static const uint8_t SPLASH_FINAL_COUNT = 10;
static uint8_t splash_count = 0;
static bool splash_finished = false;

void timer_splash(lv_timer_t * timer) {
    if (splash_finished) {
        return;
    }
    if (splash_count >= SPLASH_FINAL_COUNT) {
        print_background();
        print_frames();
        // start_snake(); // bugado
        start_battery_status(); // funcionando
        start_output_status(); // bugado

        lv_timer_pause(timer);
        splash_finished = true;
        return;
    }
    print_splash();
    splash_count++;
}

void set_colors() {
    set_splash_num_color(0x38b764u);
    set_splash_bg_color(0x257179u);
    
    set_snake_num_color(0xa7f070u);
    set_snake_bg_color(0x566c86u);

    set_snake_color_0(0x3b5dc9u);
    set_snake_color_1(0xb13e53u);
    set_snake_color_2(0x38b764u);
    set_snake_color_3(0xef7d57u);
    set_snake_color_4(0xffcd75u);
    set_snake_color_5(0x41a6f6u);
    set_snake_color_6(0xa7f070u);

    set_battery_num_color(0xa7f070u);
    set_battery_bg_color(0x566c86u);

    set_symbol_selected_color(0x38b764u);
    set_symbol_unselected_color(0xef7d57u);
    set_symbol_bg_color(0x566c86u);
    set_bt_num_color(0x38b764u);
    set_bt_bg_color(0x566c86u);

    set_frame_color(0x41a6f6u);
}

lv_obj_t* zmk_display_status_screen() {
    init_display();
    set_colors();
    
    zmk_widget_frame_init();
    zmk_widget_splash_init();

    zmk_widget_snake_init();
    zmk_widget_output_status_init();
    zmk_widget_peripheral_battery_status_init();

    lv_timer_create(timer_splash, SPLASH_DURATION, NULL);

    return lv_obj_create(NULL);
}