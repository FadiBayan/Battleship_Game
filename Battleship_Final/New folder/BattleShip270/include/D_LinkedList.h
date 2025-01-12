#ifndef D_LINKEDLIST
#define D_LINKEDLIST

typedef struct D_ListNode{

    struct D_ListNode * next;
    struct D_ListNode * prev;

    void * data;

} D_ListNode;

typedef struct{

    D_ListNode * head;
    D_ListNode * tail;

    int size;

} D_LinkedList;


D_LinkedList* create_empty_Dlist();

int is_empty(D_LinkedList* list);

void addFirst(D_LinkedList* list, void* data);

void* removeFirst(D_LinkedList* list);

void* removeLast(D_LinkedList* list);

void* peek(D_LinkedList* list);

void addLast(D_LinkedList* list, void* data);

void free_D_LinkedList(D_LinkedList* list);



#endif