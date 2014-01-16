#ifndef NSCANF_H
#define NSCANF_H

/*
 * nscanf.h
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

#include <stdarg.h>
#include <stdio.h>

/*
 * Macro to generate "field_widths" argument.
 *
 * Example use:
 *
 * int ret;
 * char numerator[20], denominator[20];
 *
 * ret = nscanf("%s/%s",
 *    NSCANF_LEN(sizeof(numerator) - 1, sizeof(denominator) - 1),
 *    numerator, denominator);
 *
 */
#define NSCANF_LEN(...)	\
	(const ssize_t []) { __VA_ARGS__ }

/*
 * field_widths is an array of ssize_t elements that specify the length
 * of each following parameter (field width). For string conversion ('s'
 * and '[' conversion specifiers), this size does _not_ include the
 * final \0 for each string. A negative size entry value does not add
 * field width information to the associated conversion specification.
 *
 * The following scanf(3) features are not accepted. Those will return -1
 * with a EINVAL error:
 * - 'n' conversion specification,
 * - 'a' conversion specification,
 * - Explicit maximum field width in the format string.
 *
 * Other than this, it behaves in the same way as the scanf(3) family of
 * functions.
 */
extern int nscanf(const char *format,
		const ssize_t *field_widths, ...)
	__attribute__ ((__format__ (__scanf__, 1, 3)));

extern int fnscanf(FILE *stream, const char *format,
		const ssize_t *field_widths, ...)
	__attribute__ ((__format__ (__scanf__, 2, 4)));

extern int snscanf(const char *str, const char *format,
		const ssize_t *field_widths, ...)
	__attribute__ ((__format__ (__scanf__, 2, 4)));

extern int vnscanf(const char *format,
		const ssize_t *field_widths, va_list ap)
	__attribute__ ((__format__ (__scanf__, 1, 0)));

extern int vfnscanf(FILE *stream, const char *format,
		const ssize_t *field_widths, va_list ap)
	__attribute__ ((__format__ (__scanf__, 2, 0)));

extern int vsnscanf(const char *str, const char *format,
		const ssize_t *field_widths, va_list ap)
	__attribute__ ((__format__ (__scanf__, 2, 0)));

#endif /* NSCANF_H */
