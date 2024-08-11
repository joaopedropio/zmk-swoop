/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * Based on ST7789V sample:
 * Copyright (c) 2019 Marc Reilly
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sample, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

#include "snake.h"

// ############## SPEED ############

typedef enum {
    SPEED_SLOW,
    SPEED_MEDIUM,
    SPEED_FAST,
    SPEED_SUPER_FAST,
} Speed;

const uint8_t WPM_SLOW = 10;
const uint8_t WPM_MEDIUM = 20;
const uint8_t WPM_FAST = 40;
const uint8_t WPM_SUPER_FAST = 80;

const uint8_t TIMER_CYCLES_SLOW = 4;
const uint8_t TIMER_CYCLES_MEDIUM = 3;
const uint8_t TIMER_CYCLES_FAST = 2;
const uint8_t TIMER_CYCLES_SUPER_FAST = 1;

static uint8_t current_cycle_speed = TIMER_CYCLES_SLOW;

static uint8_t cycles_count = 0;

static Speed current_speed = SPEED_SLOW;
static bool speed_changed = false;

// ############## DISPLAY STATICS ##############

static const struct device *display_dev;

// black buffer
static uint8_t *buf;
static struct display_buffer_descriptor buf_desc;
static size_t buf_size = 0;

// white buffer
static uint8_t *buf_white;
static struct display_buffer_descriptor buf_white_desc;
static size_t buf_white_size = 0;

// color buffer
static uint8_t *buf_color_0;
static uint8_t *buf_color_1;
static uint8_t *buf_color_2;
static uint8_t *buf_color_3;
static uint8_t *buf_color_4;
static uint8_t colors_count = 5;
static struct display_buffer_descriptor buf_color_desc;
static size_t buf_color_size = 0;
static uint8_t current_color = 0;

static void fill_buffer_snake(uint8_t *buf, size_t buf_size, uint32_t color)
{
	for (size_t idx = 0; idx < buf_size; idx += 2) {
		*(buf + idx + 0) = (color >> 8) & 0xFFu;
		*(buf + idx + 1) = (color >> 0) & 0xFFu;
	}
}

// ############## SNAKE GAME ###################

#define SNAKE_X_OFFSET     0
#define SNAKE_Y_OFFSET     0
#define SNAKE_BOARD_WIDTH  15
#define SNAKE_BOARD_HEIGHT 16
#define SNAKE_PIXEL_SIZE   16

#define SNAKE_WALK_DURATION 20
#define FATNESS             1

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    DIRECTION_LENGTH,
    DIRECTION_NONE
} Direction;

typedef struct {
    uint8_t x;
    uint8_t y;
} Snake_coordinate;

typedef struct {
    Snake_coordinate coordinate;
    Direction direction;
    uint16_t number;
} Surrounding;

typedef struct {
    Surrounding surroundings[DIRECTION_LENGTH];
} Surroundings;

typedef enum {
    HEAD,
    TAIL
} Snake_part;

typedef struct {
    Snake_coordinate coordinate;
    Snake_part part;
} Draw_step;

static bool snake_initialized = false;
static bool snake_died = false;

static uint16_t snake_board[SNAKE_BOARD_WIDTH][SNAKE_BOARD_HEIGHT];
const uint16_t out_of_board_number = 0;
const uint16_t inside_board_number = 1;

static uint16_t current_number;
static Snake_coordinate tail_coordinate;
static Snake_coordinate head_coordinate;


uint8_t forward_steps = 4;
uint8_t turn_steps = 4;
uint8_t draw_steps_length = 24; //  (forward_steps + turn_steps + turn_steps) * 2
Draw_step draw_steps[24]; // draw_steps[draw_steps_length]
static uint8_t draw_index;
static uint8_t walk_index;
static uint32_t walk_timer;

static uint8_t tail_shrink_timeout = 0;


static uint8_t random_number(uint8_t end) {
    return rand() % end;
}

static uint16_t head_number(void) {
    return snake_board[head_coordinate.x][head_coordinate.y];
}

static uint16_t tail_number(void) {
    return snake_board[tail_coordinate.x][tail_coordinate.y];
}

static uint8_t* next_color() {
    current_color++;
    if (current_color >= colors_count) {
        current_color = 0;
    }
    switch(current_color) {
        case 0: return buf_color_0;
        case 1: return buf_color_1;
        case 2: return buf_color_2;
        case 3: return buf_color_3;
        case 4: return buf_color_4;
    }
    return buf;
}

static Direction next_direction(Direction d) {
    switch (d) {
        case UP: return RIGHT;
        case RIGHT: return DOWN;
        case DOWN: return LEFT;
        case LEFT: return UP;
        case DIRECTION_LENGTH: return DIRECTION_LENGTH;
        case DIRECTION_NONE: return DIRECTION_NONE;
    }
    return DIRECTION_NONE;
}

static Direction previous_direction(Direction d) {
    switch (d) {
        case UP: return LEFT;
        case LEFT: return DOWN;
        case DOWN: return RIGHT;
        case RIGHT: return UP;
        case DIRECTION_LENGTH: return DIRECTION_LENGTH;
        case DIRECTION_NONE: return DIRECTION_NONE;
    }
    return DIRECTION_NONE;
}

static bool is_out_of_board(uint8_t x, uint8_t y) {
    return x < 0 || x >= SNAKE_BOARD_WIDTH || y < 0 || y >= SNAKE_BOARD_HEIGHT;
}

static bool is_snake_body(uint8_t x, uint8_t y) {
    return snake_board[x][y] >= tail_number() && snake_board[x][y] <= head_number();
}

static bool can_move(Direction d, uint8_t x, uint8_t y) {
    switch (d) {
        case UP: y++; break;
        case DOWN: y--; break;
        case RIGHT: x++; break;
        case LEFT: x--; break;
        case DIRECTION_LENGTH: break;
        case DIRECTION_NONE: break;
    }
    if (is_out_of_board(x, y)) {
        return false;
    }
    if (is_snake_body(x, y)) {
        return false;
    }
    return true;
}

static uint16_t scan_surrounding_number(uint8_t x, uint8_t y) {
    if (is_out_of_board(x, y)) {
        return out_of_board_number;
    }
    return snake_board[x][y];
}

static Surrounding scan_surrounding(Direction d, uint8_t x, uint8_t y) {
    Surrounding surrounding;
    Snake_coordinate coordinate;
    switch (d) {
        case UP: y++; break;
        case DOWN: y--; break;
        case RIGHT: x++; break;
        case LEFT: x--; break;
        case DIRECTION_LENGTH: break;
        case DIRECTION_NONE: break;
    }
    surrounding.direction = d;
    surrounding.number = scan_surrounding_number(x, y);
    coordinate.x = x;
    coordinate.y = y;
    surrounding.coordinate = coordinate;
    return surrounding;
}

static Surroundings scan_surroundings(uint8_t x, uint8_t y) {
    Surroundings surroundings;

    for (uint8_t dir = 0; dir < DIRECTION_LENGTH; dir++) {
        surroundings.surroundings[dir] = scan_surrounding(dir, x, y);
    }

    return surroundings;
}

static Direction neck_direction(void) {
    Surroundings surroundings = scan_surroundings(head_coordinate.x, head_coordinate.y);
    uint16_t neck_number = head_number() - 1;
    for (uint8_t dir = 0; dir < DIRECTION_LENGTH; dir++) {
        if (surroundings.surroundings[dir].number == neck_number) {
            return surroundings.surroundings[dir].direction;
        }
    }
    return DIRECTION_NONE;
}

static Direction head_direction(void) {
    Direction d = neck_direction();
    switch (d) {
        case UP: return DOWN;
        case DOWN: return UP;
        case RIGHT: return LEFT;
        case LEFT: return RIGHT;
        case DIRECTION_LENGTH: return DIRECTION_LENGTH;
        case DIRECTION_NONE: return DIRECTION_NONE;
    }
    return DIRECTION_NONE;
}

static void set_draw_step(Snake_coordinate c, Snake_part part) {
    Draw_step draw_step;
    
    draw_step.part = part;
    draw_step.coordinate = c;

    draw_steps[draw_index] = draw_step;
    draw_index++;
}

static uint16_t next_number(void) {
    current_number += 1;
    return current_number;
}

static void move_tail(void) {
    if (tail_shrink_timeout > 0) {
        tail_shrink_timeout--;
        return;
    }

    Surroundings surroundings = scan_surroundings(tail_coordinate.x, tail_coordinate.y);
    for (int dir = 0; dir < DIRECTION_LENGTH; dir++) {
        if (surroundings.surroundings[dir].number == tail_number() + 1) {
            set_draw_step(tail_coordinate, TAIL);
            Snake_coordinate c = surroundings.surroundings[dir].coordinate;
            tail_coordinate.x = c.x;
            tail_coordinate.y = c.y;
            return;
        }
    }
}

static void move_head(Direction d) {
    uint8_t x = head_coordinate.x;
    uint8_t y = head_coordinate.y;
    if (can_move(d, x, y)) {
        switch (d) {
            case UP: y++; break;
            case DOWN: y--; break;
            case RIGHT: x++; break;
            case LEFT: x--; break;
            case DIRECTION_NONE: break;
            case DIRECTION_LENGTH: break;
        }
        head_coordinate.x = x;
        head_coordinate.y = y;
        snake_board[x][y] = next_number();
        set_draw_step(head_coordinate, HEAD);
    }
}

static void move(Direction d) {
    if (can_move(d, head_coordinate.x, head_coordinate.y)) {
        move_head(d);
        move_tail();
    }
}

static void move_steps(Direction d, uint8_t steps) {
    for (int i = 0; i < steps; i++) {
        move(d);
    }
}

static void move_forward(uint8_t steps) {
    move_steps(head_direction(), steps);
}

static void turn_left(uint8_t steps) {
    move_steps(previous_direction(head_direction()), steps);
}

static void turn_right(uint8_t steps) {
    move_steps(next_direction(head_direction()), steps);
}

static void turn_random(uint8_t steps) {
    if (random_number(2) == 0) {
        return turn_left(steps);
    }
    return turn_right(steps);
}

static bool locked(void) {
    uint8_t count_moves = 0;
    for (int dir = 0; dir < DIRECTION_LENGTH; dir++) {
        if (can_move(dir, head_coordinate.x, head_coordinate.y)) {
            count_moves++;
        }
    }
    return count_moves == 0;
}

static void snake_render_pixel(uint8_t x, uint8_t y, bool on) {
    uint16_t initial_y = (y * SNAKE_PIXEL_SIZE) + SNAKE_Y_OFFSET;
    uint16_t initial_x = (x * SNAKE_PIXEL_SIZE) + SNAKE_X_OFFSET;
	if (on) {
        if (current_speed == SPEED_SUPER_FAST) {
		    display_write(display_dev, initial_x, initial_y, &buf_color_desc, next_color());
        } else {
		    display_write(display_dev, initial_x, initial_y, &buf_white_desc, buf_white);
        }
	} else {
		display_write(display_dev, initial_x, initial_y, &buf_desc, buf);
	}
}

static void draw_food(void) {
    snake_render_pixel(head_coordinate.x, head_coordinate.y, true);
}

static void make_path_to_food(void) {
    move_forward(forward_steps);
    turn_random(turn_steps);
    turn_random(turn_steps);
    tail_shrink_timeout += FATNESS;
    draw_food();
    if (locked()) {
        snake_died = true;
    }
}

static void clear_board(void) {
    for (uint8_t x = 0; x < SNAKE_BOARD_WIDTH; x++) {
        for (uint8_t y = 0; y < SNAKE_BOARD_HEIGHT; y++) {
            snake_board[x][y] = inside_board_number;
            snake_render_pixel(x, y, false);
        }
    }
}

static void initialize_snake(void) {
    clear_board();
    current_number = inside_board_number + 1;
    snake_board[4][4] = next_number();
    snake_board[4][5] = next_number();
    snake_board[4][6] = next_number();
    snake_render_pixel(4, 4, true);
    snake_render_pixel(4, 5, true);
    snake_render_pixel(4, 6, true);
    head_coordinate.x = 4;
    head_coordinate.y = 6;
    tail_coordinate.x = 4;
    tail_coordinate.y = 4;
    draw_index = 0;
    walk_index = 0;
    walk_timer = 0;
    tail_shrink_timeout = 0;
    snake_died = false;
    snake_initialized = true;
}

static void finalize_snake(void) {
    snake_initialized = false;
}

static void walk_render(void) {
    if (walk_index >= draw_index) {
        return;
    }
    Draw_step draw_step = draw_steps[walk_index];
    bool on = false;
    if (draw_step.part == HEAD) {
        on = true;
    }
    snake_render_pixel(draw_step.coordinate.x, draw_step.coordinate.y, on);
    walk_index++;
}

static void render_snake(void) {
    if (!snake_initialized) {
        initialize_snake();
        make_path_to_food();
    }
    walk_render();
    if (walk_index >= draw_index) {
        if (snake_died) {
            finalize_snake();
            return;
        }
        draw_index = 0;
        walk_index = 0;
        make_path_to_food();
    }
}

// ############## Display setup ################

void color_buffer_init() {
	buf_color_size = SNAKE_PIXEL_SIZE * SNAKE_PIXEL_SIZE * 2u;
	buf_color_0 = k_malloc(buf_color_size);
	buf_color_1 = k_malloc(buf_color_size);
	buf_color_2 = k_malloc(buf_color_size);
	buf_color_3 = k_malloc(buf_color_size);
	buf_color_4 = k_malloc(buf_color_size);
	buf_color_desc.pitch = SNAKE_PIXEL_SIZE;
	buf_color_desc.width = SNAKE_PIXEL_SIZE;
	buf_color_desc.height = SNAKE_PIXEL_SIZE;
	fill_buffer_snake(buf_color_0, buf_color_size, 0x1379u);
	fill_buffer_snake(buf_color_1, buf_color_size, 0x1111u);
	fill_buffer_snake(buf_color_2, buf_color_size, 0x4444u);
	fill_buffer_snake(buf_color_3, buf_color_size, 0x8888u);
	fill_buffer_snake(buf_color_4, buf_color_size, 0xccccu);
}

void white_buffer_init() {
	buf_white_size = SNAKE_PIXEL_SIZE * SNAKE_PIXEL_SIZE * 2u;
	buf_white = k_malloc(buf_white_size);
	buf_white_desc.pitch = SNAKE_PIXEL_SIZE;
	buf_white_desc.width = SNAKE_PIXEL_SIZE;
	buf_white_desc.height = SNAKE_PIXEL_SIZE;
	fill_buffer_snake(buf_white, buf_white_size, 0xFFFFu);
}

void buffer_init() {
	buf_size = SNAKE_PIXEL_SIZE * SNAKE_PIXEL_SIZE * 2u;
	buf = k_malloc(buf_size);
	buf_desc.pitch = SNAKE_PIXEL_SIZE;
	buf_desc.width = SNAKE_PIXEL_SIZE;
	buf_desc.height = SNAKE_PIXEL_SIZE;
	fill_buffer_snake(buf, buf_size, 0x0000u);
}


void display_setup(void) {
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device %s not found. Aborting sample.", display_dev->name);
		return;
	}
	white_buffer_init();
	buffer_init();
    color_buffer_init();
}

// wpm 

struct snake_wpm_status_state {
    uint8_t wpm;
};

Speed get_speed(uint8_t wpm) {
    if (wpm > WPM_SUPER_FAST) {
        return SPEED_SUPER_FAST;
    }
    if (wpm > WPM_FAST) {
        return SPEED_FAST;
    }
    if (wpm > WPM_MEDIUM) {
        return SPEED_MEDIUM;
    }
    return SPEED_SLOW;
}

static void set_speed(struct snake_wpm_status_state state) {
    current_speed = get_speed(state.wpm);
    switch(current_speed) {
        case SPEED_SLOW: current_cycle_speed = TIMER_CYCLES_SLOW; break;
        case SPEED_MEDIUM: current_cycle_speed = TIMER_CYCLES_MEDIUM; break;
        case SPEED_FAST: current_cycle_speed = TIMER_CYCLES_FAST; break;
        case SPEED_SUPER_FAST: current_cycle_speed = TIMER_CYCLES_SUPER_FAST; break;
    }
    speed_changed = true;
}

struct snake_wpm_status_state snake_wpm_status_get_state(const zmk_event_t *eh) {
    struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(eh);
    return (struct snake_wpm_status_state) { .wpm = ev->state };
}

void snake_wpm_status_update_cb(struct snake_wpm_status_state state) {
    set_speed(state); 
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_snake, struct snake_wpm_status_state,
                            snake_wpm_status_update_cb, snake_wpm_status_get_state)

ZMK_SUBSCRIPTION(widget_snake, zmk_wpm_state_changed);


void my_timer(lv_timer_t * timer) {
    if (speed_changed) {
        speed_changed = false;
        cycles_count = 0;
        render_snake();
    } else if (cycles_count >= current_cycle_speed) {
        cycles_count = 0;
        render_snake();
    }
    cycles_count++;
}

int zmk_widget_snake_init(struct zmk_widget_snake *widget, lv_obj_t *parent) {
    widget->obj = lv_canvas_create(parent);

    widget_snake_init();
	display_setup();
    lv_timer_t * timer = lv_timer_create(my_timer, SNAKE_WALK_DURATION, NULL);

    return 0;
}

lv_obj_t *zmk_widget_snake_obj(struct zmk_widget_snake *widget) {
    return widget->obj;
}
