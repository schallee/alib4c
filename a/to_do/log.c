#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <a/varg.h>
#include <a/log.h>
#include <a/util.h>
#include <a/error.h>
#include <a/cc.h>
#include <a/alloca.h>

#include <stdio.h>

#define A_LOG_EVENT_ONLY_CATEGORY	1
#define A_LOG_EVENT_ONLY_EVENT		1

#if HAVE_SYSLOG_H
#	include<syslog.h>
#	if HAVE_VSYSLOG && !HAVE_DECL_VSYSLOG && !defined(vsyslog)
		void vsyslog(int priority, const char *format, va_list ap);
#	endif
#endif

#define sls a_log_state
#if TARGET_WIN32
	struct event_log_state
	{
		char *barename;
		HANDLE handle;
	};
#endif

struct file_state
{
	FILE *file;
};

struct 
{
	unsigned int type;
	union
	{
#		if TARGET_WIN32
			struct event_log_state event_log;
#		endif
		struct file_state file;
	} type_state;
} sls;

a_error_t a_log_init_syslog(const char *path,
#	if TARGET_WIN32
		int A_CC_UNUSED_VAR facility
#	else
		int facility
#	endif
	)
{
	sls.type = A_LOG_SYSLOG;
#	if TARGET_UNIX
		/* open syslog */
		openlog(a_basename(path), LOG_CONS | LOG_NDELAY | LOG_PID, facility);

#	elif TARGET_WIN32
		/* cache bare name */
		if(!(sls.type_state.event_log.barename = a_barename(path)))
			return a_error_last();

		/* connect to the event log */
		if(!(sls.type_state.event_log.handle = RegisterEventSource(NULL, (LPCTSTR)(sls.type_state.event_log.barename))))
		{
			a_flail_win_su(a_error_register_event_source);
			a_cleanup(A_CLEANUP_FREE, sls.type_state.event_log.barename);
			return a_error_last();
		}
#	else
#		error syslog not implemented for anything but unix yet
#	endif
	return 0;
}

a_error_t a_log_init_stderr(void)
{
	sls.type = A_LOG_STDERR;
	return 0;
}

a_error_t a_log_init_null(void)
{
	sls.type = A_LOG_NULL;
	return 0;
}

a_error_t a_log_init_file(const char* filename)
{
	sls.type = A_LOG_FILE;

	/* open the file */
	if(!(sls.type_state.file.file = fopen(filename, "a+")))
	{
		a_flail_stdc(NULL);
		return a_flail_su(a_error_open_log_file, "log file %s", filename);
	}

	return 0;
}

a_error_t a_log_cleanup_syslog(void)
{
#	if TARGET_UNIX
		closelog();
#	elif TARGET_WIN32
		if(sls.type_state.event_log.barename)
			free(sls.type_state.event_log.barename);

		if(sls.type_state.event_log.handle && !DeregisterEventSource(sls.type_state.event_log.handle))
			return a_flail_win(a_error_deregister_event_source, NULL);
#	endif
	return 0;
}

a_error_t a_log_cleanup_file(void)
{
	if(fclose(sls.type_state.file.file))
		return a_flail_stdc(a_error_close_log_file, NULL);
	return 0;
}

a_error_t a_log_cleanup(void)
{
	switch(sls.type)
	{
		case A_LOG_SYSLOG:
			return a_log_cleanup_syslog();
		case A_LOG_STDERR:
			return 0;
		case A_LOG_NULL:
			return 0;
		case A_LOG_FILE:
			return a_log_cleanup_file();
		default:
			return a_flail_su(a_error_invalid_op, NULL);
	}
}

a_error_t a_log_str_syslog(a_log_level prio, const char *str)
{
#	if TARGET_UNIX
		syslog(prio, "%s", str);
		return 0;
#	elif TARGET_WIN32
		LPCTSTR insert_strings[1];
		static const WORD prio2category[] = 
		{
			(WORD)a_event_log_cat_emergancy,
			(WORD)a_event_log_cat_alert,
			(WORD)a_event_log_cat_critical,
			(WORD)a_event_log_cat_error,
			(WORD)a_event_log_cat_warning,
			(WORD)a_event_log_cat_notice,
			(WORD)a_event_log_cat_info,
			(WORD)a_event_log_cat_debug
		};
		static const DWORD prio2event[] =
		{
			a_event_log_msg_error,		/* emergancy */
			a_event_log_msg_error,		/* alert */
			a_event_log_msg_error,		/* critical */
			a_event_log_msg_error,		/* error */
			a_event_log_msg_warn,		/* warning */
			a_event_log_msg_warn,		/* notice */
			a_event_log_msg_info,		/* info */
			a_event_log_msg_success	/* debug */
		};
		static const WORD prio2type[] =
		{
			EVENTLOG_ERROR_TYPE,		/* emergancy */
			EVENTLOG_ERROR_TYPE,		/* alert */
			EVENTLOG_ERROR_TYPE,		/* critical */
			EVENTLOG_ERROR_TYPE,		/* error */
			EVENTLOG_WARNING_TYPE,		/* warning */
			EVENTLOG_WARNING_TYPE,		/* notice */
			EVENTLOG_INFORMATION_TYPE,	/* info */
			EVENTLOG_SUCCESS		/* debug */
		};

		/* check that prio is in range */
		if(prio > 7)
			return a_flail_su(a_error_invalid_op);

		/* make sure str isn't to big */
		if(strlen(str) > 32 * 1024)
			return a_flail_su(a_error_to_large);
	
		/* set up insert_strings */
		insert_strings[0] = str;

		/* log the event... */
		if(!ReportEvent(sls.type_state.event_log.handle, 
					prio2type[prio],
					prio2category[prio],
					prio2event[prio],
					NULL,
					1,
					0,
					insert_strings,
					NULL))
			return a_flail_win(a_error_report_event);

		return 0;
#	endif
}

a_error_t a_log_syslog(a_log_level prio, const char *fmt, va_list arg_list)
{
#	if TARGET_UNIX && ( HAVE_VSYSLOG || defined(vsyslog) )
		vsyslog(prio, fmt, arg_list);
		return 0;
#	else
		char *tmp;

		/* format the string */
		if(!(tmp = a_vsprintf_malloc(fmt, arg_list)))
			return a_error_last();

		/* log it */
		if(!a_log_str_syslog(prio, tmp))
		{
			a_free(tmp);
			a_error_last();
		}

		/* free string */
		a_free(tmp);

		return 0;
#	endif
}

a_error_t a_log_str_stderr(a_log_level A_CC_UNUSED_VAR prio, const char *str)
{
	if(fputs(str, stderr) == EOF)
		return a_flail_sdtc_su(a_error_append_log_file);
	if(fputc('\n', stderr) == EOF)
		return a_flail_sdtc_su(a_error_append_log_file);
	return 0;
}

a_error_t a_log_stderr(a_log_level A_CC_UNUSED_VAR prio, const char *fmt, va_list arg_list)
{
	if(vfprintf(stderr, fmt, arg_list)<0)
		return a_flail_stdc_su(a_error_format_msg);
	return 0;
}

a_error_t a_log_vargs(a_log_level prio, const char *fmt, va_list arg_list)
{
	switch(sls.type)
	{
		case A_LOG_SYSLOG:
			return a_log_syslog(prio, fmt, arg_list);
		case A_LOG_STDERR:
			return a_log_stderr(prio, fmt, arg_list);
		case A_LOG_NULL:
			return_success();
		default:
			return_a_error(a_error_internal_var);
	}
}

a_error_t a_log(a_log_level prio, const char *fmt, ...)
{
	va_list arg_list;

	va_start(arg_list, fmt);
	a_log_vargs(prio, fmt, arg_list);
	va_end(arg_list);

	return_previous();
}

a_error_t a_log_str(a_log_level prio, const char *str)
{
	switch(sls.type)
	{
		case A_LOG_SYSLOG:
			return a_log_str_syslog(prio, str);
		case A_LOG_STDERR:
			return a_log_str_stderr(prio, str);
		case A_LOG_NULL:
			return_success();
		default:
			return_a_error(a_error_internal_var);
	}
}

a_error_t a_log_error_vargs(a_log_level prio, const char *fmt, va_list arg_list)
{
	char *formatted;

	/* format the error */
	if(!(formatted = a_fmterror_vargs(fmt, arg_list)))
		return_previous();

	/* log it */
	a_log_str(prio, formatted);

	/* free message*/
	free(formatted);

	/* return any error a_log_str reported */
	return_previous();
}

a_error_t a_log_error(a_log_level prio, const char *fmt, ...)
{
	va_list arg_list;

	va_start(arg_list, fmt);
	a_log_error_vargs(prio, fmt, arg_list);
	va_end(arg_list);

	return_previous();
}

a_error_t a_log_error_str(a_log_level prio)
{
	char *error;

	if(!(error = a_strerror()))
		return_previous();

	if(a_log_str(prio, error))
		return_previous();

	return_success();
}

#if	TARGET_WIN32
	a_error_t a_log_install(const char *name)
	{
		HKEY regkey;
		int retval;
		char *reg_path;
		char *barename;
		DWORD tmp;
	
		/* get name of program */
		if(!(barename = a_barename(name)))
			return_previous();
	
		/* allocate space for the registry path */
		if(!(reg_path = (char *)alloca(sizeof(char) * (strlen(A_LOG_EVENT_REG_KEY) + strlen(barename) + 1))))
			return_flail("c;f", barename);
	
		/* create the registry path */
		strcpy(reg_path, A_LOG_EVENT_REG_KEY);
		strcat(reg_path, barename);
	
		/* don't need barename any more */
		a_free(barename);
	
		/* create or open the key */
		if((retval = RegCreateKey(
						HKEY_LOCAL_MACHINE,
						reg_path, 
						&regkey
					)) != ERROR_SUCCESS)
			return_flail("v;;p error opening registry key %s", retval, rename, reg_path);
	
		/* set the message file */
		if((retval = RegSetValueEx
					(
				 		regkey,
						"EventMessageFile",
						0,
						REG_EXPAND_SZ,
						(LPBYTE)a_event_log_msg_dll_loc,
						strlen(reg_path)
					)) != ERROR_SUCCESS)
			return_flail("v;;p error setting registry value %s\\EventMessageFile", retval, rename, reg_path);
	
		/* set category message file */
		if((retval = RegSetValueEx
					(
				 		regkey,
						"CategoryMessageFile",
						0,
						REG_EXPAND_SZ,
						(LPBYTE)a_event_log_msg_dll_loc,
						strlen(reg_path)
					)) != ERROR_SUCCESS)
			return_flail("v;;p error setting registry value %s\\CategoryMessageFile", retval, reg_path);
	
		/* log types */
		tmp = EVENTLOG_INFORMATION_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_ERROR_TYPE;
		if((retval = RegSetValueEx
					(
				 		regkey,
						"TypesSupported",
						0,
						REG_DWORD,
						(LPBYTE) &tmp,
						sizeof(DWORD)
					)) != ERROR_SUCCESS)
			return_flail("v;;p error setting registry value %s\\TypesSupported", retval, reg_path);
	
		/* number of categories */
		tmp = 8;
		if((retval = RegSetValueEx
					(
				 		regkey,
						"CategoryCount",
						0,
						REG_DWORD,
						(LPBYTE) &tmp,
						sizeof(DWORD)
					)) != ERROR_SUCCESS)
			return_flail("v;;p error setting registry value %s\\CategoryCount", retval, reg_path);
	
		/* close the registry */
		if((retval = RegCloseKey(regkey)) != ERROR_SUCCESS)
			return_flail("v;;p error closing registry key %s", retval, reg_path);
		
		/* done */
		return_success();
	}
	
	a_error_t a_log_uninstall(const char *name)
	{
		HKEY regkey;
		int retval;
		char *reg_path;
		char *barename;
	
		/* get name of program */
		if(!(barename = a_barename(name)))
			return_previous();
	
		/* allocate space for the registry path */
		if(!(reg_path = (char *)alloca(sizeof(char) * (strlen(A_LOG_EVENT_REG_KEY) + strlen(barename) + 1))))
			return_flail("c;f", barename);
	
		/* create the registry path */
		strcpy(reg_path, A_LOG_EVENT_REG_KEY);
		strcat(reg_path, barename);
	
		/* open registry key */
		if((retval = RegOpenKeyEx
					(
				 		HKEY_LOCAL_MACHINE,
						reg_path, 
						0,
						KEY_WRITE,
						&regkey
					)) != ERROR_SUCCESS)
			return_flail("v;f;p error opening registry key %s", retval, barename, reg_path);
	
		/* delete message file */
		if((retval = RegDeleteValue
					(
				 		regkey,
						"EventMessageFile"
					)) != ERROR_SUCCESS)
			return_flail("v;f;p deleting registry value %s\\EventMessageFile", retval, barename, reg_path);
	
		/* delete category message file */
		if((retval = RegDeleteValue
					(
				 		regkey,
						"CategoryMessageFile"
					)) != ERROR_SUCCESS)
			return_flail("v;f;p deleting registry value %s\\CategoryMessageFile", retval, barename, reg_path);
	
		/* delete types supported */
		if((retval = RegDeleteValue
					(
				 		regkey,
						"TypesSupported"
					)) != ERROR_SUCCESS)
			return_flail("v;f;p deleting registry value %s\\TypesSupported", retval, barename, reg_path);
	
		/* delete counts */
		if((retval = RegDeleteValue
					(
				 		regkey,
						"CategoryCount"
					)) != ERROR_SUCCESS)
			return_flail("v;f;p deleting registry value %s\\CategoryCount", retval, barename, reg_path);
	
		/* close registry key */
		if((retval = RegCloseKey(regkey)) != ERROR_SUCCESS)
			return_flail("v;f;p closing registry key %s", retval, barename, reg_path);
	
		/* open parent key */
		if((retval = RegOpenKeyEx
					(
				 		HKEY_LOCAL_MACHINE,
						A_LOG_EVENT_REG_KEY, 
						0,
						KEY_WRITE,
						&regkey
					)) != ERROR_SUCCESS)
			return_flail("v;f;p opening registry key " A_LOG_EVENT_REG_KEY, retval, barename);
	
		/* delete the key for our app */
		if((retval = RegDeleteKey
					(
				 		regkey,
						barename
					)) != ERROR_SUCCESS)
			return_flail("v;f;p deleteing registry key %s", retval, barename, reg_path);
	
		/* don't need barename any more */
		a_free(barename);
	
		/* close registry */
		if((retval = RegCloseKey(regkey)) != ERROR_SUCCESS)
			return_flail("v;;p closing registry key " A_LOG_EVENT_REG_KEY, retval);
	
		/* done */
		return_success();
	}
#else
#	ifndef a_log_install
		a_error_t a_log_install(const char A_CC_UNUSED_VAR *name)
		{
			return_success();
		}
#	endif

#	ifndef a_log_uninstall
		a_error_t a_log_uninstall(const char A_CC_UNUSED_VAR *name)
		{
			return_success();
		}
#	endif
#endif
