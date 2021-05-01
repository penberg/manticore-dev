#include <pthread.h>

#include <internal/uthread.h>

int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void*), void *restrict arg)
{
	start_routine(arg);

	return 0;
}

int pthread_join(pthread_t thread, void **value_ptr)
{
	return 0;
}
