#include <sutil/flail.h>

int main(int argc, char **argv)
{
	flail_init(argv[0]);
	flail("qvo;test %s", 0, "testing");
	return 0;
}
