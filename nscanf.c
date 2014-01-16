/*
 * nscanf.c
 *
 * Secure scanf wrapper
 *
 * Copyright (c) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>

#include "nscanf.h"

/* Max width of size_t printed as string (2^64) */
#define NSCANF_NUM_WIDTH	20

/*
 * If returns 0, caller needs to free format_out.
 */
static
int nscanf_prepare_fmt(const char *format,
		char **format_out,
		const size_t *field_widths)
{
	char *format_a, *dest_p;
	size_t format_a_len;
	const char *p, *prev_p;
	int ret;
	const size_t *fw_iter = field_widths;
	size_t nr_fw = 0;

	/*
	 * Calculate number of field width to learn the format
	 * allocation length we need.
	 */
	p = format;

	while (*p != '\0') {
		p = strchrnul(p, '%');
		if (*p == '\0') {
			break;
		}
		p++;	/* Include % */
		if (*p == '%') {
			/* match %% */
			p++;
			continue;
		}
		if (isdigit(*p)) {
			/* unexpected numerical field len */
			errno = EINVAL;
			return -1;
		}
		/* Found one location for field width */
		nr_fw++;
	}
	assert(strlen(format) == p - format);

	/* Generate the format string with field width info */
	format_a_len = p - format + 1 + (nr_fw * NSCANF_NUM_WIDTH);
	format_a = malloc(format_a_len);
	if (!format_a) {
		errno = ENOMEM;
		return -1;
	}
	format_a[0] = '\0';

	p = format;
	dest_p = format_a;

	while (*p != '\0') {
		prev_p = p;
		p = strchrnul(p, '%');
		if (*p == '\0') {
			memcpy(dest_p, prev_p, p - prev_p);
			break;
		}
		p++;	/* Include % */
		memcpy(dest_p, prev_p, p - prev_p);
		dest_p += p - prev_p;
		if (*p == '%') {
			/* match %% */
			*(dest_p++) = '%';
			p++;
			continue;
		}
		/*
		 * Print len into format string. Size received as input
		 * includes final \0.
		 */
		ret = sprintf(dest_p, "%zu", *(fw_iter++) - 1);
		if (ret < 0) {
			goto end_error;
		}
		assert(ret <= NSCANF_NUM_WIDTH);
		dest_p += ret;
	}

	*format_out = format_a;
end:
	return 0;

end_error:
	free(format_a);
	return ret;
}

int vsnscanf(const char *str, const char *format,
		const size_t *field_widths, va_list ap)
{
	char *format_a;
	int ret;

	ret = nscanf_prepare_fmt(format, &format_a, field_widths);
	if (ret < 0) {
		goto end;
	}
	ret = vsscanf(str, format_a, ap);
	free(format_a);
end:
	return ret;
}

int snscanf(const char *str, const char *format,
		const size_t *field_widths, ...)
{
	va_list ap;
	int ret;

	va_start(ap, field_widths);
	ret = vsnscanf(str, format, field_widths, ap);
	va_end(ap);
	return ret;
}

int vfnscanf(FILE *stream, const char *format,
		const size_t *field_widths, va_list ap)
{
	char *format_a;
	int ret;

	ret = nscanf_prepare_fmt(format, &format_a, field_widths);
	if (ret < 0) {
		goto end;
	}
	ret = vfscanf(stream, format_a, ap);
	free(format_a);
end:
	return ret;
}

int fnscanf(FILE *stream, const char *format,
		const size_t *field_widths, ...)
{
	va_list ap;
	int ret;

	va_start(ap, field_widths);
	ret = vfnscanf(stream, format, field_widths, ap);
	va_end(ap);
	return ret;
}

int vnscanf(const char *format,
		const size_t *field_widths, va_list ap)
{
	return vfnscanf(stdin, format, field_widths, ap);
}

int nscanf(const char *format, const size_t *field_widths, ...)
{
	va_list ap;
	int ret;

	va_start(ap, field_widths);
	ret = vfnscanf(stdin, format, field_widths, ap);
	va_end(ap);
	return ret;
}
