#include <su/daemon.h>
#include <su/cc.h>
#include <su/types.h>

bool init_callback(void *nothing SU_UNUSED_VAR)
{

}

int main(void)
{
	if(su_log_init_syslog("daemon", su_log_daemon))
		su_flail("
	su_daemon("daemon", 
}
