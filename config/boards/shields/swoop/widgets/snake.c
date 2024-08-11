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
#include "snake.h"

#define CANVAS_WIDTH       240
#define CANVAS_HEIGHT      240
#define SNAKE_BOARD_WIDTH  30
#define SNAKE_BOARD_HEIGHT 30
#define SNAKE_PIXEL_SIZE   8

#define SNAKE_WALK_DURATION 40
#define FATNESS             2

static lv_obj_t * canvas;
static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(CANVAS_WIDTH, CANVAS_HEIGHT)];
static lv_color_t c0;
static lv_color_t c1;
static lv_color_t black;
static lv_color_t white;

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

bool snake_initialized = false;
bool snake_died = false;

uint16_t snake_board[SNAKE_BOARD_WIDTH][SNAKE_BOARD_HEIGHT];
const uint16_t out_of_board_number = 0;
const uint16_t inside_board_number = 1;

uint16_t current_number;
Snake_coordinate tail_coordinate;
Snake_coordinate head_coordinate;


uint8_t forward_steps = 4;
uint8_t turn_steps = 4;
uint8_t draw_steps_length = 24; //  (forward_steps + turn_steps + turn_steps) * 2
Draw_step draw_steps[24]; // draw_steps[draw_steps_length]
uint8_t draw_index;
uint8_t walk_index;
uint32_t walk_timer;

uint8_t tail_shrink_timeout = 0;


static uint8_t random_number(uint8_t end) {
    return rand() % end;
}

static uint16_t head_number(void) {
    return snake_board[head_coordinate.x][head_coordinate.y];
}

static uint16_t tail_number(void) {
    return snake_board[tail_coordinate.x][tail_coordinate.y];
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
    uint8_t initial_y = y * SNAKE_PIXEL_SIZE;
    uint8_t initial_x = x * SNAKE_PIXEL_SIZE;
    for (uint8_t i = initial_y; i < initial_y + SNAKE_PIXEL_SIZE; ++i) {
        for (uint8_t j = initial_x; j < initial_x + SNAKE_PIXEL_SIZE; ++j) {
            if (on) {
                lv_canvas_set_px_color(canvas, j, i, c0);
            } else {
                lv_canvas_set_px_color(canvas, j, i, c1);
            }
        }
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
    snake_board[4][20] = next_number();
    snake_board[4][21] = next_number();
    snake_board[4][22] = next_number();
    snake_render_pixel(4, 20, true);
    snake_render_pixel(4, 21, true);
    snake_render_pixel(4, 22, true);
    head_coordinate.x = 4;
    head_coordinate.y = 22;
    tail_coordinate.x = 4;
    tail_coordinate.y = 20;
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

void my_timer(lv_timer_t * timer)
{
    render_snake();
}

int zmk_widget_snake_init(struct zmk_widget_snake *widget, lv_obj_t *parent) {

    canvas = lv_canvas_create(parent);
    widget->obj = canvas;

    /*Create colors with the indices of the palette*/
    black = lv_color_hex(0x000000);
    white = lv_color_hex(0xffffff);
    c0.full = 0;
    c1.full = 1;

    /*Create a canvas and initialize its palette*/
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_INDEXED_1BIT);
    lv_canvas_set_palette(canvas, 0, white);
    lv_canvas_set_palette(canvas, 1, black);
    /*Red background (There is no dedicated alpha channel in indexed images so LV_OPA_COVER is ignored)*/
    lv_canvas_fill_bg(canvas, c1, LV_OPA_COVER);

    /*Create hole on the canvas*/

    lv_timer_t * timer = lv_timer_create(my_timer, SNAKE_WALK_DURATION, NULL);
    //lv_timer_ready(timer);
    //lv_timer_set_repeat_count(timer, 20)

    return 0;
}

lv_obj_t *zmk_widget_snake_obj(struct zmk_widget_snake *widget) {
    return widget->obj;
}