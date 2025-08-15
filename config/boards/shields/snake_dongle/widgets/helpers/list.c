#include <stdlib.h>
#include <zephyr/kernel.h>
#include "list.h"

Snake_List* create_list() {
    Snake_List *list = (Snake_List *)malloc(sizeof(Snake_List));
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

Snake_Node* create_node(uint8_t x, uint8_t y) {
    Snake_Node *node = (Snake_Node *)malloc(sizeof(Snake_Node));
    node->x = x;
    node->y = y;
    return node;
}

bool empty_list(Snake_List *list) {
    return list->head == NULL && list->tail == NULL;
}

uint8_t list_length(Snake_List *list) {
    Snake_Node *node;
    uint8_t count;
    if (list->head == NULL && list->tail == NULL) {
        return 0;
    }
    node = list->head;
    count = 1;
    while(node != list->tail) {
        node = node->next;
        count++;
    }
    return count;
}

void prepend(Snake_List *list, uint8_t x, uint8_t y) {
    Snake_Node *node = create_node(x, y);
    if (empty_list(list)) {
        list->head = node;
        list->tail = node;

        node->next = node;
        node->prev = node;
        
        return;
    }

    node->next = list->head;
    node->prev = list->tail;

    list->head->prev = node;
    list->tail->next = node;

    list->head = node;
}

void remove_tail(Snake_List *list) {
    if (empty_list(list)) {
        return;
    }
    if (list->head == list->tail) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        return;
    }
    list->head->prev = list->tail->prev;
    list->tail->prev->next = list->head;
    free(list->tail);
    list->tail = list->head->prev;
}

void clean_list(Snake_List *list) {
    while(list_length(list) > 0) {
        remove_tail(list);
    }
}
