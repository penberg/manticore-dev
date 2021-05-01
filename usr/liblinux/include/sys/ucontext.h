#ifndef __LIBLINUX_UCONTEXT_H
#define __LIBLINUX_UCONTEXT_H

#ifdef __x86_64__
typedef struct {
	void *ss_sp;
	int ss_flags;
	size_t ss_size;
} stack_t;

typedef long long int greg_t;

#define __NGREG 23

typedef greg_t gregset_t[__NGREG];

typedef struct {
	gregset_t gregs;
} mcontext_t;

typedef struct ucontext_t {
	stack_t uc_stack;
	mcontext_t uc_mcontext;
} ucontext_t;

void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);

int swapcontext(ucontext_t *restrict oucp, const ucontext_t *restrict ucp);

int getcontext(ucontext_t *ucp);

int setcontext(const ucontext_t *ucp);

#else
#error "Architecture is not supported."
#endif

#endif
