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
 *    NSCANF_LEN(sizeof(numerator), sizeof(denominator)),
 *    numerator, denominator);
 *
 */
#define NSCANF_LEN(...)	\
	(const ssize_t []) { __VA_ARGS__ }

/*
 * Common integer character max widths.
 */
#define NSCANF_LEN_u32	10
#define NSCANF_LEN_s32	11
#define NSCANF_LEN_o32	11
#define NSCANF_LEN_x32	9
#define NSCANF_LEN_u64	20
#define NSCANF_LEN_s64	21
#define NSCANF_LEN_o64	22
#define NSCANF_LEN_x64	17

/*
 * field_widths is an array of ssize_t elements that specify the length
 * of each following parameter (field width).
 *
 * UNLIKE TO THE REGULAR SCANF(3) FAMILY OF FUNCTIONS, FOR SECURITY
 * REASONS, STRING CONVERSION ('s' AND '[' CONVERSION SPECIFIERS) WIDTH
 * INCLUDE THE FINAL \0 FOR EACH STRING. FOR THOSE, A WIDTH OF 0 RETURN
 * -1 WITH A EINVAL ERROR.
 *
 * The following scanf(3) features are not accepted. Those return -1
 * with a EINVAL error:
 * - 'n' conversion specification,
 * - 'a' conversion specification,
 * - Explicit maximum field width in the format string.
 *
 * A negative field width value skips field width validation for the
 * associated conversion specification (use with care, only with
 * pre-validated inputs!).

 * Otherwise, the nscanf() family of functions behaves in the same way
 * as scanf(3).
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
