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
#include "dino.h"

#define SRC(array) (const void **)array, sizeof(array) / sizeof(lv_img_dsc_t *)

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

LV_IMG_DECLARE(dino1);

#define ANIMATION_SPEED_DINO 200
const lv_img_dsc_t *imgs_dino[] = {
    &dino1,
};


int zmk_widget_dino_init(struct zmk_widget_dino *widget, lv_obj_t *parent) {
    widget->obj = lv_animimg_create(parent);
    lv_obj_center(widget->obj);

    sys_slist_append(&widgets, &widget->node);

    lv_animimg_set_src(widget->obj, SRC(imgs_dino));
    lv_animimg_set_duration(widget->obj, ANIMATION_SPEED_DINO);
    lv_animimg_set_repeat_count(widget->obj, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(widget->obj);

    return 0;
}

lv_obj_t *zmk_widget_dino_obj(struct zmk_widget_dino *widget) {
    return widget->obj;
}
