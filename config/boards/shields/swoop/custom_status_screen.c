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
        start_snake(); // bugado
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

    // https://lospec.com/palette-list/fuzzyfour good 
    // uint32_t color1 = 0x302387u;
    // uint32_t color2 = 0xff3796u;
    // uint32_t color3 = 0x00faacu;
    // uint32_t color4 = 0xfffdafu;
    
    // https://lospec.com/palette-list/lava-gb not so much
    // uint32_t color1 = 0x051f39u;
    // uint32_t color2 = 0x4a2480u;
    // uint32_t color3 = 0xc53a9du;
    // uint32_t color4 = 0xff8e80u;

    // https://lospec.com/palette-list/kirokaze-gameboy good
    // uint32_t color1 = 0x332c50u;
    // uint32_t color2 = 0x46878fu;
    // uint32_t color3 = 0x94e344u;
    // uint32_t color4 = 0xe2f3e4u;

    // https://lospec.com/palette-list/moonlight-gb
    // uint32_t color1 = 0x0f052du;
    // uint32_t color2 = 0x203671u;
    // uint32_t color3 = 0x36868fu;
    // uint32_t color4 = 0x5fc75du;

    // https://lospec.com/palette-list/cherrymelon so so
    // uint32_t color1 = 0xfcdeeau;
    // uint32_t color2 = 0xff4d6du;
    // uint32_t color3 = 0x265935u;
    // uint32_t color4 = 0x012824u;

    // // https://lospec.com/palette-list/hollow not so much
    // uint32_t color1 = 0x0f0f1bu;
    // uint32_t color2 = 0x565a75u;
    // uint32_t color3 = 0xc6b7beu;
    // uint32_t color4 = 0xfafbf6u;

    // https://lospec.com/palette-list/bittersweet
    // uint32_t color1 = 0x282328u;
    // uint32_t color2 = 0x545c7eu;
    // uint32_t color3 = 0xc56981u;
    // uint32_t color4 = 0xa3a29au;

    // https://lospec.com/palette-list/b4sement best one so far
    uint32_t color1 = 0x222323u;
    uint32_t color2 = 0xff4adcu;
    uint32_t color3 = 0x3dff98u;
    uint32_t color4 = 0xf0f6f0u;

    // https://lospec.com/palette-list/americana
    // uint32_t color1 = 0xfce4a8u;
    // uint32_t color2 = 0x71969fu;
    // uint32_t color3 = 0xd71a21u;
    // uint32_t color4 = 0x01334eu;

    // ######################## colors #################
    
    uint32_t black = 0x000000u;
    uint32_t black_1 = 0xddddddu;

    set_splash_num_color(color2);
    set_splash_bg_color(color1);
    
    set_snake_font_color(color2);
    set_snake_num_color(color3);
    set_snake_bg_color(color1);

    set_snake_default_color(color3);
    set_snake_board_color(color1);
    set_snake_board_1_color(black);

    set_food_color(color2);
    set_snake_color_0(color2);
    set_snake_color_1(color3);
    set_snake_color_2(color4);
    set_snake_color_3(color2);
    set_snake_color_4(color3);
    set_snake_color_5(color4);
    set_snake_color_6(color2);

    set_battery_num_color(color3);
    set_battery_percentage_color(color2);
    set_battery_bg_color(color1);

    set_symbol_selected_color(color3);
    set_symbol_unselected_color(color2);
    set_symbol_bg_color(color1);

    set_bt_num_color(color4);
    set_bt_bg_color(color1);

    set_frame_color(color4);
    set_frame_color_1(black_1);
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