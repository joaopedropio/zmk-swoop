/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "glorp_gif.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

LV_IMG_DECLARE(glorp_gif_descriptor);

int zmk_widget_glorp_gif_init(struct zmk_widget_glorp_gif *widget, lv_obj_t *parent) {
    widget->obj = lv_gif_create(parent);
    lv_obj_center(widget->obj);

    sys_slist_append(&widgets, &widget->node);

    lv_gif_set_src(widget->obj, &glorp_gif_descriptor);

    return 0;
}

lv_obj_t *zmk_widget_glorp_gif_obj(struct zmk_widget_glorp_gif *widget) {
    return widget->obj;
}

/*int zmk_widget_bongo_cat_fast_init(struct zmk_widget_bongo_cat_fast *widget, lv_obj_t *parent) {
    widget->obj = lv_animimg_create(parent);
    lv_obj_center(widget->obj);

    sys_slist_append(&widgets, &widget->node);

    lv_animimg_set_src(widget->obj, SRC(fast_imgs));
    lv_animimg_set_duration(widget->obj, ANIMATION_SPEED_FAST);
    lv_animimg_set_repeat_count(widget->obj, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(widget->obj);
    return 0;
}*/