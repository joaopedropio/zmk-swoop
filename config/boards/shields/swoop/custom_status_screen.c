/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "battery_status.h"
#include "bongo_cat_fast.h"
#include "glorp_gif.h"
#include "custom_status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_battery_statusx battery_status_widget;
static struct zmk_widget_bongo_cat_fast bongo_cat_widget;
static struct zmk_widget_glorp_gif glorp_gif_widget;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;
    screen = lv_obj_create(NULL);

    zmk_widget_battery_statusx_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_statusx_obj(&battery_status_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);

    zmk_widget_bongo_cat_fast_init(&bongo_cat_widget, screen);
    lv_obj_align(zmk_widget_bongo_cat_fast_obj(&bongo_cat_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    zmk_widget_glorp_gif_init(&glorp_gif_widget, screen);
    lv_obj_align(zmk_widget_glorp_gif_obj(&glorp_gif_widget), LV_ALIGN_CENTER, 0, 0);

    return screen;
}