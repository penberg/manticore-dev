#ifndef __LIBLINUX_INTERNAL_SOCKET_H
#define __LIBLINUX_INTERNAL_SOCKET_H

#include <stddef.h>
#include <stdint.h>

#include <sys/socket.h>

#include "internal/net.h"

struct socket;

struct socket_operations {
	ssize_t (*recvfrom)(struct socket *sk, void *restrict buf, size_t len, int flags,
			    struct sockaddr *restrict src_addr, socklen_t *restrict addrlen);
	ssize_t (*sendto)(struct socket *sk, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr,
			  socklen_t addrlen);
};

struct ip_fragment {
	void *data;
	size_t remaining;
	uint16_t offset;
	uint16_t len;
};

#define MAX_FRAGMENTS 8 /* FIXME */

struct ip_datagram {
	uint32_t host;
	uint16_t port;
	uint16_t id;
	uint16_t offset;
	struct ip_fragment fragments[MAX_FRAGMENTS];
	size_t nr_fragments;
};

bool ip_datagram_append(struct ip_datagram *datagram, void *data, uint16_t offset, uint16_t len);

struct ip_fragment *datagram_next_fragment(struct ip_datagram *datagram);

size_t ip_fragment_consume(struct ip_fragment *fragment, void *buf, size_t len);

#define MAX_DATAGRAMS 8 /* FIXME */

struct socket {
	const struct socket_operations *ops;
	uint16_t local_port;
	struct ip_datagram datagrams[MAX_DATAGRAMS];
	char rx_buffer[1500]; /* FIXME make bigger */
};

int socket_alloc(int domain, int type, int protocol);

struct socket *socket_lookup_by_fd(int sockfd);
struct socket *socket_lookup_by_flow(uint16_t local_port, uint16_t foreign_port);
struct ip_datagram *socket_get_datagram(struct socket *socket, uint16_t id);
struct ip_datagram *socket_next_datagram(struct socket *socket);

void* socket_input(struct socket *sk, struct packet_view *pk);

int socket_accept(struct socket *sk, struct sockaddr *restrict addr, socklen_t *restrict addrlen);
int socket_bind(struct socket *sk, const struct sockaddr *addr, socklen_t addrlen);
int socket_getsockopt(struct socket *sk, int level, int optname, void *restrict optval, socklen_t *restrict optlen);
int socket_listen(struct socket *sk, int backlog);
ssize_t socket_recvfrom(struct socket *sk, void *restrict buf, size_t len, int flags,
			struct sockaddr *restrict src_addr, socklen_t *restrict addrlen);
ssize_t socket_sendto(struct socket *sk, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr,
		      socklen_t addrlen);
int socket_setsockopt(struct socket *sk, int level, int optname, const void *optval, socklen_t optlen);
int socket_shutdown(struct socket *sk, int how);

#endif
