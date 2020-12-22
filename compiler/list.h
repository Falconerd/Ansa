#ifndef list_h_INCLUDED
#define list_h_INCLUDED

#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

typedef struct list {
	size_t item_size;
	uint64_t length;
	uint64_t capacity;
	void* head;
} List;

void* list_add(List* list, void* item) {
	assert(list->item_size > 0);

	if (!list->capacity) {
		list->head = malloc(8 * list->item_size);
		list->capacity = 8;
	} else if (list->capacity == list->length) {
		void* new_list_data = realloc(list->head, list->item_size * list->length * 2);
		assert(new_list_data);
		list->head = new_list_data;
		list->capacity *= 2;
	}

	uint8_t* element = (uint8_t*) list->head+(list->item_size * list->length++);
	memcpy(element, item, list->item_size);

	return (void*) element;
}

#endif

