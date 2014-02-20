/*
 * nscanf-test.c
 *
 * Secure scanf wrapper test
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
