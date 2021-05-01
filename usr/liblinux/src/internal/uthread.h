#ifndef __LIBLINUX_INTERNAL_UTHREAD_H
#define __LIBLINUX_INTERNAL_UTHREAD_H

#include <sys/ucontext.h>

/// User-level thread control block.
struct uthread {
	ucontext_t context;
};

struct uthread* uthread_new(void);

void uthread_delete(void);

#endif
