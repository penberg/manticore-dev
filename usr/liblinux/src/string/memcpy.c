#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	printf("%s(dest=%p, src=%p, n=%lu\n", __func__, dest ,src, n);
	const char *s = src;
	char *d = dest;

	for (size_t i = 0; i < n; i++) {
		d[i] = s[i];
	}
	return dest;
}
