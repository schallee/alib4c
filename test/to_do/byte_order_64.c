#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <test_common.h>
#include <su/util.h>
#include <su/types.h>
#include <su/error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void);

#define test_file "byte_order_64.dat1"

int main(void)
{
	uint64_t h = 1;
	uint64_t n;
	uint64_t f;
	FILE *file;

	verb_printf("1 in host format: %llu\n", h);

	n = hton_64(h);
	verb_printf("1 in network format: %llu\n", n);

	f = ntoh_64(n);
	verb_printf("1 in host format after conversion: %llu\n", f);

	if(f != h)
		return TEST_FAIL;

	if(!(file = open_testfile(test_file)))
		test_fail_flail(";;r");

	if(fread(&n, sizeof(n), 1, file) != 1)
		test_fail_flail("us;;r error reading from %s", su_error_file_read, test_file);

	if(fclose(file))
		test_fail_flail("us;;r error closing %s", su_error_file_close, test_file);

	f = ntoh_64(n);
	verb_printf("42 converted from network byte order is: %llu\n", f);
	if(f != 42)
		test_fail();

	test_pass();
}
