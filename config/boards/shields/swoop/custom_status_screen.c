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
#include "widgets/helpers/font.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const uint8_t SPLASH_DURATION = 50;
static const uint8_t SPLASH_FINAL_COUNT = 10;
static uint8_t splash_count = 0;

void my_timer_splash(lv_timer_t * timer) {
    if (splash_count >= SPLASH_FINAL_COUNT) {
        zmk_widget_snake_init();
        run_snake();

        zmk_widget_output_status_init();

        zmk_widget_peripheral_battery_status_init();

        lv_timer_pause(timer);
        return;
    }
    print_splash();
    splash_count++;
}

void snake() {
    zmk_widget_splash_init();
    lv_timer_create(my_timer_splash, SPLASH_DURATION, NULL);
}

lv_obj_t* zmk_display_status_screen() {
    init_display();
    snake();

    return lv_obj_create(NULL);
}