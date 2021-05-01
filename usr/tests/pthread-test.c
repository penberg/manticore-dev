#include <assert.h>
#include <pthread.h>
#include <stdio.h>

static void *run_thread(void *arg)
{
	fprintf(stdout, "hello, pthread!\n");

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thread;

	assert(pthread_create(&thread, NULL, run_thread, NULL) == 0);

	assert(pthread_join(thread, NULL) == 0);

	return 0;
}
