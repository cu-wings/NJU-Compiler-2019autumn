#ifndef _COMPILER_LIST_H_
#define _COMPILER_LIST_H_
#include <assert.h>
#include "common.h"

struct ListHead {
	struct ListHead *prev, *next;
};
typedef struct ListHead ListHead;

#define list_entry(ptr, type, member) \
	((type*)((char*)(ptr) - (long)(&((type*)0)->member)))

static inline void
list_add(ListHead *prev, ListHead *next, ListHead *data) {
	assert(data != NULL);
	data->prev = prev;
	data->next = next;
	if (prev != NULL) prev->next = data;
	if (next != NULL) next->prev = data;
}

static inline void
list_add_before(ListHead *list, ListHead *data) {
	assert(list != NULL);
	list_add(list->prev, list, data);
}

static inline void
list_add_after(ListHead *list, ListHead *data) {
	assert(list != NULL);
	list_add(list, list->next, data);
}

static inline void
list_del(ListHead *data) {
	assert(data != NULL);
	ListHead *prev = data->prev;
	ListHead *next = data->next;
	if (prev != NULL) prev->next = next;
	if (next != NULL) next->prev = prev;
}

static inline void
list_init(ListHead *list) {
	assert(list != NULL);
	list->prev = list->next = list;
}

static inline bool
list_empty(ListHead *list) {
	assert(list != NULL);
	return list == list->next;
}

#define list_foreach(ptr, head) \
	for ((ptr) = (head)->next; (ptr) != (head); (ptr) = (ptr)->next)

#define list_foreach_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)  

#endif // _COMPILER_LIST_H_

