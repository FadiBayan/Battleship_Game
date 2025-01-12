#include <stdio.h>
#include <stdlib.h>
#include "D_LinkedList.h"

D_LinkedList* create_empty_Dlist() {
    D_LinkedList * list = (D_LinkedList*)malloc(sizeof(D_LinkedList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

int is_empty(D_LinkedList* list) {
    return list->head == NULL;
}

void addFirst(D_LinkedList* list, void* data) {
    D_ListNode* new_node = (D_ListNode*)malloc(sizeof(D_ListNode));
    new_node->data = data;
    new_node->next = list->head;

    if (list->head == NULL){
        list->tail = new_node;
    }
    else{
        list->head->prev = new_node;
    }

    list->head = new_node;

    list->size++;
}

void* removeFirst(D_LinkedList* list) {
    if (is_empty(list)) {
        return NULL; 
    }
    D_ListNode* temp = list->head;
    void* data = temp->data;
    list->head = list->head->next;
    if (list->head != NULL){
        list->head->prev = NULL;
    }
    else {
        list->tail = NULL;
    }
    free(temp);

    list->size--;

    return data;
}

void* removeLast(D_LinkedList* list) {
    if (is_empty(list)) {
        return NULL; 
    }
    D_ListNode* temp = list->tail;
    void* data = temp->data;
    list->tail = list->tail->prev;
    if (list->tail != NULL){
        list->tail->next = NULL;
    }
    else {
        list->head = NULL;
    }
    free(temp);

    list->size--;

    return data;
}

void* peek(D_LinkedList* list) {
    if (is_empty(list)) {
        return NULL; 
    }
    return list->head->data;
}

void addLast(D_LinkedList* list, void* data) {
    D_ListNode* new_node = (D_ListNode*)malloc(sizeof(D_ListNode));
    new_node->data = data;
    new_node->next = NULL;

    if (is_empty(list)) {
        list->head = new_node;
    } else {
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->size++;
}

void free_D_LinkedList(D_LinkedList* list) {
    while (!is_empty(list)) {
        removeFirst(list);
    }
    free(list);
}