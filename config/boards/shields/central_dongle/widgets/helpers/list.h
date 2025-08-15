#pragma once

#include <zephyr/kernel.h>

typedef struct Snake_Node {
    uint8_t x;
    uint8_t y;

    struct Snake_Node* next;
    struct Snake_Node* prev;

} Snake_Node;

typedef struct Snake_List {
    uint8_t length;
    struct Snake_Node* head;
    struct Snake_Node* tail;
} Snake_List;

Snake_List* create_list();
uint8_t list_length(Snake_List *list);
void prepend(Snake_List *list, uint8_t x, uint8_t y);
void remove_tail(Snake_List *list);
void clean_list(Snake_List *list);