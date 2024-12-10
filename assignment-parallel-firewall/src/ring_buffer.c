// SPDX-License-Identifier: BSD-3-Clause

#include "ring_buffer.h"

int ring_buffer_init(so_ring_buffer_t *ring, size_t cap)
{
	/* TODO: implement ring_buffer_init */
	ring->data = malloc(cap);
	if (!ring->data)
		abort();
	ring->read_pos = 0;
	ring->write_pos = 0;
	ring->len = 0;
	ring->cap = cap;
	ring->stop = 0;
	if (pthread_mutex_init(&ring->mutex, NULL) != 0)
		abort();
	if (pthread_cond_init(&ring->must_produce, NULL) != 0)
		abort();
	if (pthread_cond_init(&ring->must_consume, NULL) != 0)
		abort();
	return 1;
}

ssize_t ring_buffer_enqueue(so_ring_buffer_t *ring, void *data, size_t size)
{
	/* TODO: implement ring_buffer_enqueue */
	pthread_mutex_lock(&ring->mutex);
	while (ring->len + size > ring->cap) {
		if (ring->stop) {
			pthread_mutex_unlock(&ring->mutex);
			return 0;
		}
		pthread_cond_wait(&ring->must_produce, &ring->mutex);
	}
	size_t available_size = ring->cap - ring->write_pos;
	size_t to_copy = size > available_size ? available_size : size;

	memcpy(ring->data + ring->write_pos, data, to_copy);
	if (to_copy != size)
		memcpy(ring->data, (char *)data + to_copy, size - to_copy);
	ring->write_pos = (ring->write_pos + size) % ring->cap;
	ring->len += size;
	pthread_cond_broadcast(&ring->must_consume);
	pthread_mutex_unlock(&ring->mutex);
	return size;
}

ssize_t ring_buffer_dequeue(so_ring_buffer_t *ring, void *data, size_t size)
{
	/* TODO: Implement ring_buffer_dequeue */
	pthread_mutex_lock(&ring->mutex);
	while (ring->len < size) {
		if (ring->stop) {
			pthread_mutex_unlock(&ring->mutex);
			return 0;
		}
		pthread_cond_wait(&ring->must_consume, &ring->mutex);
	}
	size_t available = ring->cap - ring->read_pos;
	size_t to_copy = size > available ? available : size;

	memcpy(data, ring->data + ring->read_pos, to_copy);
	if (to_copy != size)
		memcpy((char *)data + to_copy, ring->data, size - to_copy);
	ring->read_pos = (ring->read_pos + size) % ring->cap;
	ring->len -= size;
	pthread_cond_signal(&ring->must_produce);
	pthread_mutex_unlock(&ring->mutex);
	return size;
}

void ring_buffer_destroy(so_ring_buffer_t *ring)
{
	/* TODO: Implement ring_buffer_destroy */
	pthread_mutex_destroy(&ring->mutex);
	pthread_cond_destroy(&ring->must_consume);
	pthread_cond_destroy(&ring->must_produce);
	free(ring->data);
	free(ring);
}

void ring_buffer_stop(so_ring_buffer_t *ring)
{
	/* TODO: Implement ring_buffer_stop */
	pthread_mutex_lock(&ring->mutex);
	ring->stop = 1;
	pthread_cond_broadcast(&ring->must_consume);
	pthread_mutex_unlock(&ring->mutex);
}
