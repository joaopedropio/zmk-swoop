/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_battery_statusx {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_battery_statusx_init(struct zmk_widget_battery_statusx *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_battery_statusx_obj(struct zmk_widget_battery_statusx *widget);