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
extern int unique_rand(const int *prev, size_t prev_len);
extern bool contains(const int *array, size_t len, int val);
extern int nonzero_rand();

#endif
