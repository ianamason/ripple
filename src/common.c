#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "common.h"

extern struct options_t options;

void mem_assign(
		uint8_t *buf,
		const size_t buf_sz,
		const uint64_t val,
		const size_t val_sz)
{
	if (val_sz != 1 && val_sz != 2 && val_sz != 4 && val_sz != 8) {
		fprintf(stderr, "%s: val_sz must be 1, 2, 4, or 8\n", __func__);
		exit(EXIT_FAILURE);
	}

	if ((buf_sz % val_sz) != 0) {
		fprintf(stderr, "%s: buf_sz must be multiple of val_sz\n", __func__);
		exit(EXIT_FAILURE);
	}

	if (val_sz == 1)
		memset(buf, val, buf_sz);
	 else
		for (uint64_t i = 0; i < buf_sz; i += val_sz)
			memcpy(buf + i, &val, val_sz);
}

void* xmalloc(
		const size_t n)
{
	void *ptr = malloc(n);

	if (ptr) return ptr;

	perror("malloc");
	abort();
}

void* xrealloc(
		void *ptr,
		const size_t n)
{
	void *p = realloc(ptr, n);

	if (p) return p;

	perror("realloc");
	abort();
}

const
size_t read_data(
		const int fd,
		uint8_t *const buf,
		const size_t buf_sz)
{
	size_t len = 0;
	while (len < buf_sz) {
		const ssize_t ret = read(fd, buf + len, buf_sz - len);

		if (ret < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		if (ret == 0) break;

		len += ret;
	}

	return len;
}

void write_data(
		const int fd,
		const uint8_t *const buf,
		const size_t sz)
{
	size_t written;

	for (written = 0; written < sz;) {
		const ssize_t ret = write(fd, buf + written, sz - written);

		if (ret < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}

		if (ret == 0) break;

		written += ret;
	}
}

// Should this be a macro?
void verbose_printf(
		const char *const fmt,
		...)
{
	if (!options.verbose) return;

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void verbose_dump(
		const uint8_t *const buf,
		const size_t sz,
		const unsigned long long base)
{
	if (!options.verbose) return;

	dump(buf, sz, base);
}

void dump(
		const uint8_t *const buf,
		const size_t sz,
		const unsigned long long base)
{
	for (size_t i = 0; i < sz; i += 0x10) {
		if (base != -1) printf(REGFMT ": ", base + i);

		for (size_t j = i; j < (i + 0x10); j++) {
			if (j < sz)
				printf("%02x ", buf[j]);
			else
				printf("   ");
		}

		printf("\t");

		for (size_t j = i; j < (i + 0x10) && j < sz; j++) {
			if (j < sz) {
				if (buf[j] > 0x1f && buf[j] < 0x7f)
					printf("%c", buf[j]);
				else
					printf(".");
			}
		}
		printf("\n");
	}
}

/* try and parse the string as decimal, then as hex, then give up */
uint64_t parse2uint64(
		  const char *const arg)
{
  char* end = NULL;
  int64_t val;

  if(arg == NULL)
    return 0;

  errno = 0;
  val = strtoul(arg, &end, 10);
  if (val == ULONG_MAX && errno) {
    perror("strtoul: ");
    return 0;
  }
  if(end != NULL && *end == '\0')
    return val; 

  errno = 0;
  val = strtoul(arg, &end, 16);
  if (val == ULONG_MAX && errno) {
    perror("strtoul: ");
    return 0;
  }
  if(end != NULL && *end == '\0')
    return val; 

  return 0;		
}
