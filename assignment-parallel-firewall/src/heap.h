#ifndef __SO_HEAP__
#define __SO_HEAP__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "packet.h"

typedef struct {
	unsigned long timestamp;
	char *data;
	int len;
	int height;
} MinHeapNode;

typedef struct {
	MinHeapNode **array;
	int capacity;
	int size;
} MinHeap;

MinHeap *packets;

MinHeap *create_min_heap(int capacity);

void destroy_min_heap(MinHeap *heap);

void insert_min_heap(MinHeap *heap, unsigned long timestamp, char *data, int len);

MinHeapNode *extract_min(MinHeap *heap);

void print_in_order(MinHeap *heap, int fd);

bool is_empty(MinHeap *heap);

#endif
