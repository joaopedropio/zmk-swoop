/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
 
 #pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_glorp_gif {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_glorp_gif_init(struct zmk_widget_glorp_gif *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_glorp_gif_obj(struct zmk_widget_glorp_gif *widget);