#ifndef su_daemon_h
#define du_daemon_h	1

#include <su/types.h>
#include <su/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*su_daemon_callback)(void *);
typedef bool (*su_daemon_opts_callback)(void *, int, char **);

void su_daemon(const char *name, const su_daemon_callback init, const su_daemon_callback run, const su_daemon_callback stop, const su_daemon_callback cleanup, void *arg) SU_CC_NO_RETURN;

su_error_t su_daemon_starting(void);
su_error_t su_daemon_stopping(void);
su_error_t su_daemon_install(const char *path);
su_error_t su_daemon_uninstall(const char *path);

#ifdef __cplusplus
}
#endif

#endif
