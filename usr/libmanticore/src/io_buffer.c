#include <manticore/io_buffer.h>

#include <manticore/io_queue.h>

void *io_buffer_start(struct io_buffer *iob)
{
	return iob->data + iob->offset;
}

size_t io_buffer_remaining(struct io_buffer *iob)
{
	return iob->len - iob->offset;
}

/* Acquire a reference to an I/O buffer.  */
void io_buffer_get(struct io_buffer *iob)
{
#if 0
	assert(iob->ref_count > 0);
#endif

	iob->ref_count++;
}

void io_buffer_consume_full(struct io_buffer *iob)
{
	io_buffer_consume(iob, io_buffer_remaining(iob));
}

void io_buffer_consume(struct io_buffer *iob, size_t size)
{
#if 0
	assert(size < io_buffer_remaining(iob));
#endif

	iob->offset += size;
}

/* Release a reference to an I/O buffer.  */
void io_buffer_put(struct io_buffer *iob)
{
#if 0
	assert(iob->ref_count > 0);
#endif

	iob->ref_count--;

	if (!iob->ref_count) {
		io_buffer_complete(iob);
	}
}

void io_buffer_complete(struct io_buffer *iob)
{
	io_complete(iob->queue, iob->data, iob->len);
}
