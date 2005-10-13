#ifndef a_log_h
#define a_log_h

#if HAVE_CONFIG_H
#	include <config.h>
#endif

#if HAVE_SYSLOG_H
#	include <syslog.h>
#endif

#if HAVE_WINDOWS_H
#	include <windows.h>
#endif

#include <a/varg.h>
#include <a/types.h>
#include <a/error.h>
#include <a/cc.h>

#define A_LOG_NULL	0
#define A_LOG_SYSLOG	1
#define A_LOG_STDERR	2
#define A_LOG_FILE	3

#if TARGET_WIN32
#	define A_LOG_EVENT_REG_KEY	"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"
/*#	define A_DLL_LOCATION		"%ProgramFiles%\\darkmist\\sutil\\a_event_log.dll"*/
#	define A_DLL_LOCATION		"c:\msys\1.0\usr\local\lib\liba_event_log-0.dll"
#endif

/* log facilities */
#if HAVE_SYSLOG_H
#	define a_log_authentication	LOG_AUTHPRIV
#	define a_log_daemon		LOG_DAEMON
#	define a_log_user		LOG_USER
#	define a_log_local0		LOG_LOCAL0
#	define a_log_local1		LOG_LOCAL1
#	define a_log_local2		LOG_LOCAL2
#	define a_log_local3		LOG_LOCAL3
#	define a_log_local4		LOG_LOCAL4
#	define a_log_local5		LOG_LOCAL5
#	define a_log_local6		LOG_LOCAL6
#	define a_log_local7		LOG_LOCAL7
#else
#	define a_log_authentication	0
#	define a_log_daemon		0
#	define a_log_user		0
#	define a_log_local0		0
#	define a_log_local1		0
#	define a_log_local2		0
#	define a_log_local3		0
#	define a_log_local4		0
#	define a_log_local5		0
#	define a_log_local6		0
#	define a_log_local7		0
#endif

/* log levels */
#if HAVE_SYSLOG_H
	typedef int a_log_level;
#	define a_log_level_emergancy		((a_log_level)LOG_EMERG)
#	define a_log_level_alert		((a_log_level)LOG_ALERT)
#	define a_log_level_critical		((a_log_level)LOG_CRIT)
#	define a_log_level_error		((a_log_level)LOG_ERR)
#	define a_log_level_warning		((a_log_level)LOG_WARNING)
#	define a_log_level_notice		((a_log_level)LOG_NOTICE)
#	define a_log_level_info		((a_log_level)LOG_INFO)
#	define a_log_level_debug		((a_log_level)LOG_DEBUG)
#elif	TARGET_WIN32
	typedef WORD a_log_level;
#	define a_log_level_emergancy		((a_log_level)EVENTLOG_ERROR_TYPE)
#	define a_log_level_alert		((a_log_level)EVENTLOG_ERROR_TYPE)
#	define a_log_level_critical		((a_log_level)EVENTLOG_ERROR_TYPE)
#	define a_log_level_error		((a_log_level)EVENTLOG_ERROR_TYPE)
#	define a_log_level_warning		((a_log_level)EVENTLOG_WARNING_TYPE)
#	define a_log_level_notice		((a_log_level)EVENTLOG_WARNING_TYPE)
#	define a_log_level_info		((a_log_level)EVENTLOG_INFORMATION_TYPE)
#	define a_log_level_debug		((a_log_level)EVENTLOG_INFORMATION_TYPE)
#else
	typedef uint_fast8_t a_log_level;
#	define a_log_level_emergancy		((a_log_level)0)
#	define a_log_level_alert		((a_log_level)1)
#	define a_log_level_critical		((a_log_level)2)
#	define a_log_level_error		((a_log_level)3)
#	define a_log_level_warning		((a_log_level)4)
#	define a_log_level_notice		((a_log_level)5)
#	define a_log_level_info		((a_log_level)6)
#	define a_log_level_debug		((a_log_level)7)
#endif

/* special win32 stuff */
#if TARGET_WIN32
	/* categories, one per log level */
#	define a_event_log_cat_emergancy	0xE0000001L
#	define a_event_log_cat_alert		0xE0000002L
#	define a_event_log_cat_critical	0xE0000003L
#	define a_event_log_cat_error		0xE0000004L
#	define a_event_log_cat_warning		0xA0000005L
#	define a_event_log_cat_notice		0xA0000006L
#	define a_event_log_cat_info		0x60000007L
#	define a_event_log_cat_debug		0x60000008L
	/* msgs, one per event type */
#	define a_event_log_msg_success		0x20000009L
#	define a_event_log_msg_info		0x6000000AL
#	define a_event_log_msg_warn		0xA000000BL
#	define a_event_log_msg_error		0xE000000CL
#endif

/* prototypes */
a_begin_c_decls_m

a_error_t a_log_init_null(void);
a_error_t a_log_init_syslog(const char *path, int facility);
a_error_t a_log_init_stderr(void);
a_error_t a_log_init_file(const char* filename);
a_error_t a_log_str_syslog(a_log_level prio, const char *str);
a_error_t a_log_syslog(a_log_level prio, const char *fmt, va_list arg_list);
a_error_t a_log_str_stderr(a_log_level A_CC_UNUSED_VAR prio, const char *str);
a_error_t a_log_stderr(a_log_level A_CC_UNUSED_VAR prio, const char *fmt, va_list arg_list) A_CC_PRINTF(2,0);
a_error_t a_log_vargs(a_log_level prio, const char *fmt, va_list arg_list) A_CC_PRINTF(2,0);
a_error_t a_log(a_log_level prio, const char *fmt, ...) A_CC_PRINTF(2,3);
a_error_t a_log_str(a_log_level prio, const char *str);
a_error_t a_log_error_vargs(a_log_level prio, const char *fmt, va_list arg_list);
a_error_t a_log_error(a_log_level prio, const char *fmt, ...) A_CC_PRINTF(2,3);
a_error_t a_log_error_str(a_log_level prio);
a_error_t a_log_cleanup(void);
a_error_t a_log_cleanup_syslog(void);
a_error_t a_log_cleanup_file(void);

#if TARGET_WIN32
	a_error_t a_log_install(const char *name);
	a_error_t a_log_uninstall(const char *name);
	extern const char a_event_log_msg_dll_loc[];
#else
#	define a_log_install(name)	a_error_success
#	define a_log_uninstall(name)	a_error_success
#endif

a_end_c_decls_m

#endif
