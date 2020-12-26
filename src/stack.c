#include "./stack.h"

void* stack_pop(Stack* stack) {
	if (NULL == stack->head) {
		return NULL;
	} else {
		StackItem* item = stack->head;
		stack->head = item->prev;
		--stack->count;
		return item->data;
	}
}

void* stack_push(Stack* stack, void* data) {
	StackItem* item = malloc(sizeof(StackItem));
	item->data = malloc(stack->item_size);
	memcpy(item->data, data, stack->item_size);
	item->prev = stack->head;
	stack->head = item;
	++stack->count;
	// @TODO: return something?
	return NULL;
}
