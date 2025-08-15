/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
 
 #pragma once

#include <zephyr/kernel.h>
#include <lvgl.h>

void print_background(void);
void print_splash(void);
void zmk_widget_splash_init(void);
void clean_up_splash(void);