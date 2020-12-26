#include <assert.h>
#include <stdio.h>
#include "./stack.h"

int main() {
	Stack s = {0};
	s.item_size = sizeof(int);
	int items[] = { 2, 3, 4, 5 };
	stack_push(&s, &items[0]);
	stack_push(&s, &items[1]);
	stack_push(&s, &items[2]);
	stack_push(&s, &items[3]);
	int item = *(int*) stack_pop(&s);
	assert(item == 5);
	item = *(int*) stack_pop(&s);
	assert(item == 4);
	item = *(int*) stack_pop(&s);
	assert(item == 3);
	item = *(int*) stack_pop(&s);
	assert(item == 2);
	assert(NULL == stack_pop(&s));

	stack_push(&s, &items[0]);
	stack_push(&s, &items[1]);
	assert(*(int*)stack_pop(&s) == 3);
	stack_push(&s, &items[3]);
	assert(*(int*)stack_pop(&s) == 5);
	assert(*(int*)stack_pop(&s) == 2);

	printf("\033[32mStack tests passed\033[0m\n");
	return 0;
}
