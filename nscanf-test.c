/*
 * nscanf-test.c
 *
 * Secure scanf wrapper test
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

#include "nscanf.h"

int main(int argc, char **argv)
{
	int ret;
	const char *test[] = {
		"test askdjhfaskljdfh aa % again",
		"test lkjlkjlkjlkjlkj zzzz bb %",
		"net123other",
	};
	char name[10];
	char name2[5];
	int testint;

	name[0] = '\0';
	name2[0] = '\0';
	ret = sscanf(test[0], "test %9s aa %% again", name);
	printf("%d \"%s\"\n", ret, name);

	name[0] = '\0';
	name2[0] = '\0';
	ret = sscanf(test[1], "test %9s %4s bb %%", name, name2);
	printf("%d \"%s\" \"%s\"\n", ret, name, name2);

	name[0] = '\0';
	testint = 0;
	ret = sscanf(test[2], "net%d%s", &testint, name);
	printf("%d \"%d\" \"%s\"\n", ret, testint, name);


	name[0] = '\0';
	name2[0] = '\0';
	ret = snscanf(test[0],
		"test %s aa %% again",
		NSCANF_LEN(sizeof(name)),
		name);
	printf("%d \"%s\"\n", ret, name);

	name[0] = '\0';
	name2[0] = '\0';
	ret = snscanf(test[1],
		"test %s %s bb %%",
		NSCANF_LEN(sizeof(name), sizeof(name2)),
		name, name2),
	printf("%d \"%s\" \"%s\"\n", ret, name, name2);

	name[0] = '\0';
	name2[0] = '\0';
	testint = 0;
	ret = snscanf(test[2],
		"net%d%s",
		NSCANF_LEN(-1, sizeof(name)),
		&testint, name);
	printf("%d \"%d\" \"%s\"\n", ret, testint, name);

	return 0;
}
