/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
 
 #pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_dino {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_dino_init(struct zmk_widget_dino *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_dino_obj(struct zmk_widget_dino *widget);