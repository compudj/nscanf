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
		const ssize_t *field_widths)
{
	char *format_a, *dest_p;
	size_t format_a_len;
	const char *p, *prev_p;
	int ret;
	const ssize_t *fw_iter = field_widths;
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
		switch (*p) {
		case '%':	/* match %% */
			p++;
			continue;	/* restart loop */
		case '*':	/* match %* */
			p++;
			break;		/* exit switch */
		default:
			break;		/* exit switch */
		}

		/* Match n$ part of %n$ */
		prev_p = p;
		for (;;) {
			if (*p == '\0') {
				errno = EINVAL;
				return -1;
			}
			if (isdigit(*p)) {
				p++;
				continue;
			}
			if (*p == '$') {
				p++;
				/* This is the %n$ */
				break;
			}
			/* This is not a %n$ */
			p = prev_p;
			break;
		}

		/*
		 * 'a' can be either GNU extension (dynamic allocation)
		 * or C99 conversion specifier. Refuse it due to
		 * semantic uncertainty.
		 */
		if (*p == 'a') {
			errno = EINVAL;
			return -1;
		}

		/* Refuse explicit field width */
		if (isdigit(*p)) {
			/* unexpected numerical field len */
			errno = EINVAL;
			return -1;
		}

		/* Length modifiers */
		switch (*p) {
		case 'h':
			/* 'h' or 'hh' */
			p++;
			if (*p == 'h') {
				p++;
			}
			break;		/* exit switch */
		case 'l':
			/* 'l' or 'll' */
			p++;
			if (*p == 'l') {
				p++;
			}
			break;		/* exit switch */
		case 'j':
		case 'z':
		case 't':
		case 'q':
		case 'L':
			p++;
			break;		/* exit switch */
		default:
			/* no length modifier */
			break;		/* exit switch */
		}

		/* conversion specifiers */
		switch (*p) {
		case '[':
			/*
			 * We need to understand the '[' conversion
			 * specifier because it may contain an extra %
			 * character.
			 */
			p++;
			if (*p == '^') {
				p++;
			}
			if (*p == ']') {
				p++;
			}
			p = strchrnul(p, ']');
			if (*p == '\0') {
				/* Invalid: missing ']' */
				errno = EINVAL;
				return -1;
			}
			p++;		/* skip over ']' */
			break;		/* exit switch */
		case 'n':
		case 'a':
			/* Refuse the 'n' and 'a' specifiers */
			errno = EINVAL;
			return -1;
		default:
			/*
			 * Leave other conversion specifier validation
			 * to the scanf implementation.
			 */
			break;		/* exit switch */
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

		switch (*p) {
		case '%':	/* match %% */
			*(dest_p++) = *(p++);
			continue;	/* restart loop */
		case '*':	/* match %* */
			*(dest_p++) = *(p++);
			break;		/* exit switch */
		default:
			break;		/* exit switch */
		}

		/* Match n$ part of %n$ */
		prev_p = p;
		for (;;) {
			assert(*p != '\0');
			if (isdigit(*p)) {
				p++;
				continue;
			}
			if (*p == '$') {
				p++;
				/* This is the %n$ */
				memcpy(dest_p, prev_p, p - prev_p);
				dest_p += p - prev_p;
				break;
			}
			/* This is not a %n$ */
			p = prev_p;
			break;
		}

		assert(*p != 'a');
		assert(!isdigit(*p));

		/*
		 * Print len into format string.
		 */
		if (*fw_iter >= 0) {
			size_t width = *fw_iter;

			switch (*p) {
			case 's':
			case '[':
				if (!width) {
					errno = EINVAL;
					ret = -1;
					goto end_error;
				}
				/*
				 * Provide a dumb-proof API by including
				 * the final '\0' within the width
				 * received as argument, unlike scanf().
				 * Adapt the width value for scanf()
				 * here.
				 */
				width--;
				break;
			default:
				break;
			}
			ret = sprintf(dest_p, "%zu", width);
			if (ret < 0) {
				goto end_error;
			}
			assert(ret <= NSCANF_NUM_WIDTH);
			dest_p += ret;
		}
		fw_iter++;

		/* Length modifiers */
		switch (*p) {
		case 'h':
			/* 'h' or 'hh' */
			*(dest_p++) = *(p++);
			if (*p == 'h') {
				*(dest_p++) = *(p++);
			}
			break;		/* exit switch */
		case 'l':
			/* 'l' or 'll' */
			*(dest_p++) = *(p++);
			if (*p == 'l') {
				*(dest_p++) = *(p++);
			}
			break;		/* exit switch */
		case 'j':
		case 'z':
		case 't':
		case 'q':
		case 'L':
			*(dest_p++) = *(p++);
			break;		/* exit switch */
		default:
			/* no length modifier */
			break;		/* exit switch */
		}

		/* conversion specifiers */
		assert(*p != 'n' && *p != 'a');
		prev_p = p;
		switch (*p) {
		case '[':
			/*
			 * We need to understand the '[' conversion
			 * specifier because it may contain an extra %
			 * character.
			 */
			p++;
			if (*p == '^') {
				p++;
			}
			if (*p == ']') {
				p++;
			}
			p = strchrnul(p, ']');
			assert(*p != '\0');
			p++;		/* skip over ']' */
			memcpy(dest_p, prev_p, p - prev_p);
			dest_p += p - prev_p;
			break;		/* exit switch */
		default:
			/*
			 * Leave other conversion specifier validation
			 * to the scanf implementation.
			 */
			break;		/* exit switch */
		}
	}
	*format_out = format_a;
	return 0;

end_error:
	free(format_a);
	return ret;
}

int vsnscanf(const char *str, const char *format,
		const ssize_t *field_widths, va_list ap)
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
		const ssize_t *field_widths, ...)
{
	va_list ap;
	int ret;

	va_start(ap, field_widths);
	ret = vsnscanf(str, format, field_widths, ap);
	va_end(ap);
	return ret;
}

int vfnscanf(FILE *stream, const char *format,
		const ssize_t *field_widths, va_list ap)
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
		const ssize_t *field_widths, ...)
{
	va_list ap;
	int ret;

	va_start(ap, field_widths);
	ret = vfnscanf(stream, format, field_widths, ap);
	va_end(ap);
	return ret;
}

int vnscanf(const char *format,
		const ssize_t *field_widths, va_list ap)
{
	return vfnscanf(stdin, format, field_widths, ap);
}

int nscanf(const char *format, const ssize_t *field_widths, ...)
{
	va_list ap;
	int ret;

	va_start(ap, field_widths);
	ret = vfnscanf(stdin, format, field_widths, ap);
	va_end(ap);
	return ret;
}
