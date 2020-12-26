#ifndef stack_h_INCLUDED
#define stack_h_INCLUDED
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
typedef struct stack_item StackItem;

typedef struct stack {
	size_t item_size;
	uint64_t count;
	StackItem* head;
} Stack;

struct stack_item {
	StackItem* prev;
	void* data;
};

void* stack_pop(Stack* stack);
void* stack_push(Stack* stack, void* data);
#endif
