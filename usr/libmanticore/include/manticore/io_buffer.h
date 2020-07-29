#ifndef __LIBMANTICORE_IO_BUFFER_H
#define __LIBMANTICORE_IO_BUFFER_H

#include <manticore/io_queue.h>

#include <stddef.h>
#include <stdint.h>

struct io_buffer {
	io_queue_t queue;
	uint64_t ref_count;
	void *data;
	size_t len;
	size_t offset;
};

void *io_buffer_start(struct io_buffer *iob);
size_t io_buffer_remaining(struct io_buffer *iob);
void io_buffer_get(struct io_buffer *iob);
void io_buffer_consume_full(struct io_buffer *iob);
void io_buffer_consume(struct io_buffer *iob, size_t size);
void io_buffer_put(struct io_buffer *iob);
void io_buffer_complete(struct io_buffer *iob);

#endif
