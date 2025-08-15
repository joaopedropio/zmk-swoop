/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
 
#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

void zmk_widget_peripheral_battery_status_init(void);
void start_battery_status(void);
