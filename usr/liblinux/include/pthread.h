#ifndef __LIBLINUX_PTHREAD_H
#define __LIBLINUX_PTHREAD_H

#include <sys/types.h>

int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void*), void *restrict arg);
int pthread_join(pthread_t thread, void **value_ptr);

#endif
