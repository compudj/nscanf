#ifndef NSCANF_H
#define NSCANF_H

/*
 * nscanf.h
 *
 * Secure scanf wrapper
 *
 * Copyright (c) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
 *
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
