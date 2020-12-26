#include "internal/net.h"

#include "internal/setup.h"
#include "internal/trace.h"

#include <manticore/io_buffer.h>

#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct net_statistics {
	uint64_t	packets_dropped;
};

static struct net_statistics stats;

static bool net_input_one(struct io_buffer *iob)
{
	LIBLINUX_TRACE(net_input);

	struct ethhdr *ethh = io_buffer_start(iob);

	if (io_buffer_remaining(iob) < sizeof(*ethh)) {
		io_buffer_consume_full(iob);
		stats.packets_dropped++;
		return false;
	}
	io_buffer_consume(iob, sizeof(*ethh));

	if (ethh->h_proto == ntohs(ETH_P_IP)) {
		return ip_input(iob);
	} else if (ethh->h_proto == ntohs(ETH_P_ARP)) {
		arp_input(iob);
		return false;
	} else {
		io_buffer_consume_full(iob);
		stats.packets_dropped++;
		return false;
	}
}

static void free_iobuf(struct io_buffer *iob)
{
	// TODO: should we make this delayed free for better memory reuse?
	free(iob);
}

bool net_input(struct packet_view *pk)
{
	bool ret = false;

	void *addr = pk->start;

	size_t len = pk->end - pk->start;

	struct io_buffer *iob = malloc(sizeof(struct io_buffer));
	assert(iob != NULL);
       
	*iob = (struct io_buffer) {
		.queue = __liblinux_eth_ioqueue,
		.ref_count = 1,
		.data = addr,
		.len = len,
		.offset = 0,
#if 0 // FIXME
		.complete = free_iobuf,
#endif
	};

	while (io_buffer_remaining(iob) > 0) {
		ret |= net_input_one(iob);
	}
	io_buffer_put(iob);

	return ret;
}
