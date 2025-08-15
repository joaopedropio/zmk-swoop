/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
 
 #pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

void zmk_widget_output_status_init(void);
void start_output_status(void);
void stop_output_status(void);
void set_status_symbol(void);