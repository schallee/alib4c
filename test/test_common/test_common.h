/*
 *  Copyright (C) 2012 Ed Schaller <schallee@darkmist.net>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef test_common_h
#define test_common_h
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>			/* FILE */
#include <a/error.h>

#define TEST_FAIL			1
#define TEST_PASS			0
#define TEST_SHOULD_FAIL		77
					/* 77 per automake */
#define TEST_XFAIL			TEST_SHOULD_FAIL

#define test_pass()			exit(TEST_PASS)
#define test_fail()			exit(TEST_FAIL)
#define test_should_fail()		exit(TEST_SHOULD_FAIL)
#define test_xfail()			test_should_fail()

#if USE_VERBOSE_TESTS
#	define verbose_printf(...)	printf(__VA_ARGS__)
#else
#	define verbose_printf(...)
#endif
#define verb_printf(...)		verbose_printf(__VA_ARGS__)

extern FILE *open_testfile(const char* filename);
extern int unique_rand(const intptr_t *prev, size_t prev_len);
extern bool contains(const intptr_t *array, size_t len, int val);
extern int nonzero_rand();

#endif
