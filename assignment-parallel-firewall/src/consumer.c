// SPDX-License-Identifier: BSD-3-Clause

#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "consumer.h"
#include "ring_buffer.h"
#include "packet.h"
#include "utils.h"

void consumer_thread(so_consumer_ctx_t *ctx)
{
	/* TODO: implement consumer thread */
	while (1) {
		size_t size = PKT_SZ;
		char *consumed = malloc(size);
		char aux[PKT_SZ];

		if (!ring_buffer_dequeue(ctx->producer_rb, consumed, size))
			return;
		struct so_packet_t *pkt = (struct so_packet_t *)consumed;
		int action = process_packet(pkt);
		unsigned long hash = packet_hash(pkt);
		unsigned long timestamp = pkt->hdr.timestamp;
		int len = snprintf(aux, PKT_SZ, "%s %016lx %lu\n", RES_TO_STR(action),
						   hash, timestamp);
		pthread_mutex_lock(&output_mutex);
		insert_min_heap(packets, timestamp, aux, len);
		pthread_mutex_unlock(&output_mutex);
		free(consumed);
	}
}

int create_consumers(pthread_t *tids,
					 int num_consumers,
					 struct so_ring_buffer_t *rb)
{
	pthread_mutex_init(&output_mutex, NULL);
	for (int i = 0; i < num_consumers; i++) {
		/*
		 * TODO: Launch consumer threads
		 **/
		so_consumer_ctx_t *args = malloc(sizeof(so_consumer_ctx_t));

		args->producer_rb = rb;
		if (pthread_create(&tids[i], NULL, consumer_thread, args) != 0)
			exit(1);
	}
	return num_consumers;
}
