#include "datastructures/vector.h"

void vector_init(struct Vector* v)
{
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(void*) * v->capacity);
}

int vector_total(struct Vector* v)
{
    return v->total;
}

void vector_resize(struct Vector* v, int capacity)
{
#ifdef DEBUG_ON
    printf("vector_resize: %d to %d\n", v->capacity, capacity);
#endif

    void** items = realloc(v->items, sizeof(void*) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void vector_add(struct Vector* v, void* item)
{
    if (v->capacity == v->total)
        vector_resize(v, v->capacity * 2);
    v->items[v->total++] = item;
}

void vector_set(struct Vector* v, int index, void* item)
{
    if (index >= 0 && index < v->total)
        v->items[index] = item;
}

void* vector_get(struct Vector* v, int index)
{
    if (index >= 0 && index < v->total)
        return v->items[index];
    return NULL;
}

void vector_delete(struct Vector* v, int index)
{
    if (index < 0 || index >= v->total)
        return;

    v->items[index] = NULL;

    for (int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->total--;

    if (v->total > 0 && v->total == v->capacity / 4)
        vector_resize(v, v->capacity / 2);
}

void vector_free(struct Vector* v)
{
    free(v->items);
}

void vector_clear(struct Vector* v)
{
    for (int i = v->total - 1; i >= 0; i--)
        vector_delete(v, i);
}