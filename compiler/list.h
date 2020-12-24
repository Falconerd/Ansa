#ifndef list_h_INCLUDED
#define list_h_INCLUDED
#include <inttypes.h>

typedef struct list {
	size_t item_size;
	uint64_t length;
	uint64_t capacity;
	void* head;
} List;

void* list_add(List* list, void* item);
void* list_at(List* list, uint64_t index);
#endif

