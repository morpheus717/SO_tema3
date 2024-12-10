// SPDX-License-Identifier: BSD-3-Clause

#include "heap.h"

MinHeap *create_min_heap(int capacity)
{
	MinHeap *heap = malloc(sizeof(MinHeap));

	heap->capacity = capacity;
	heap->size = 0;
	heap->array = malloc(sizeof(MinHeapNode *) * capacity);

	return heap;
}

void destroy_min_heap(MinHeap *heap)
{
	for (int i = 0; i < heap->size; i++) {
		free(heap->array[i]->data);
		free(heap->array[i]);
	}
	free(heap->array);
	free(heap);
}

void swap_nodes(MinHeapNode **x, MinHeapNode **y)
{
	MinHeapNode *temp = *x;
	*x = *y;
	*y = temp;
}

void min_heapify(MinHeap *heap, int i)
{
	int left = 2 * i + 1;
	int right = 2 * i + 2;
	int smallest = i;

	if (left < heap->size && heap->array[left]->timestamp < heap->array[smallest]->timestamp)
		smallest = left;
	if (right < heap->size && heap->array[right]->timestamp < heap->array[smallest]->timestamp)
		smallest = right;
	if (smallest != i) {
		swap_nodes(&heap->array[i], &heap->array[smallest]);
		min_heapify(heap, smallest);
	}
}

void insert_min_heap(MinHeap *heap, unsigned long timestamp, char *data, int len)
{
	if (heap->size == heap->capacity)
		return;
	MinHeapNode *new_node = malloc(sizeof(MinHeapNode));

	new_node->timestamp = timestamp;
	new_node->len = len;
	new_node->height = 1;
	new_node->data = malloc(len);
	memcpy(new_node->data, data, len);
	int i = heap->size++;

	heap->array[i] = new_node;
	while (i != 0 && heap->array[(i - 1) / 2]->timestamp > heap->array[i]->timestamp) {
		swap_nodes(&heap->array[i], &heap->array[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

MinHeapNode *extract_min(MinHeap *heap)
{
	if (heap->size <= 0)
		return NULL;
	MinHeapNode *root = heap->array[0];

	if (heap->size == 1) {
		heap->size--;
		return root;
	}
	heap->array[0] = heap->array[--heap->size];
	min_heapify(heap, 0);
	return root;
}

void print_in_order(MinHeap *heap, int fd)
{
	MinHeap temp_heap = *heap;

	while (!is_empty(&temp_heap)) {
		MinHeapNode *node = extract_min(&temp_heap);

		write(fd, node->data, node->len);
		free(node->data);
		free(node);
	}
}

bool is_empty(MinHeap *heap)
{
	return heap->size == 0;
}
