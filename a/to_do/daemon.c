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

#if HAVE_CONFIG_H
#	include <config.h>
#endif

#if HAVE_WINDOWS_H
#	include <windows.h>
#endif

#if HAVE_SIGNAL_H
#	include <signal.h>
#endif

#if HAVE_UNISTD_H
#	include <unistd.h>
#	if (HAVE_DAEMON && !HAVE_DECL_DAEMON
		int daemon(int nochdir, int noclose);
#	endif
#endif

#include <su/util.h>
#include <su/daemon.h>
#include <su/types.h>
#include <su/error.h>
#include <su/log.h>

#include <stdio.h>

#define sds		su_daemon_state
#if TARGET_WIN32
#	define WAIT_HINT	(5 * 1000)
#	define STATUS_INTERVAL	500

	/* local windows specific prototypes */
	static void WINAPI su_daemon_ctrl_handler(DWORD opcode);
	static su_error_t su_daemon_stopped(unsigned int exit_code);
	static su_error_t su_daemon_running(void);
	static void su_daemon_status_init(void);
	static void WINAPI su_daemon_service_start(DWORD argc, LPTSTR *argv);
#elif TARGET_UNIX
	/* local unix specific prototypes */
	static void su_daemon_sig_term(int n);
#endif

/* local prototypes */

static struct {
	su_daemon_callback	init;
	su_daemon_callback	run;
	su_daemon_callback	stop;
	su_daemon_callback	cleanup;
	void *			arg;
	const char *		name;
#	if TARGET_WIN32
		SERVICE_STATUS		status;
		SERVICE_STATUS_HANDLE	status_handle;
		DWORD			status_counter;
		unsigned int 		error_code;
#	endif
} sds;

#ifndef su_daemon_install
su_error_t su_daemon_install(const char 
#if !TARGET_WIN32
		SU_CC_UNUSED_VAR
#endif
		*path)
{
#	if TARGET_WIN32
		SC_HANDLE service_db;
		SC_HANDLE service;
		char *full_path;
		char *name;
	
		if(!(full_path = su_fullpath(path)))
			return_flail(";;p could not get full pathname");

		if(!(name = su_barename(path)))
			return_flail(";f;p could not get bare program name", full_path);

		if(!(service_db = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)))
			return_flail("w;ff;p could not open SCM", full_path, name);
	
		/*SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,*/
		if(!(service = CreateService(
				service_db,
				name,
				name,
				SERVICE_ALL_ACCESS|SERVICE_START|SERVICE_STOP,
				SERVICE_WIN32_OWN_PROCESS,
				SERVICE_AUTO_START,
				SERVICE_ERROR_NORMAL,
				full_path,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL)))
			return_flail("w;ff;p could not create service", full_path, name);

		su_free(full_path);
		su_free(name);
		
		if(!CloseServiceHandle(service))
			return_flail("w;;p could not close service");
	
		if(!CloseServiceHandle(service_db))
			return_flail("w;;p could not close SCM");

		if(su_log_install(path))
			return_flail("w;;p could not install event log info");

		return_success();
#	else
		return_su_error(su_error_not_implemented);
#	endif
}
#endif
	
#ifndef su_daemon_uninstall
su_error_t su_daemon_uninstall(const char
#if !TARGET_WIN32
		SU_CC_UNUSED_VAR
#endif
		*path)
{
#	if TARGET_WIN32
		SC_HANDLE service_db;
		SC_HANDLE service;
		char *name;
	
		if(!(name = su_barename(path)))
			return_flail(";;p could not get bare program name");

		if(!(service_db = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)))
			return_flail("w;f;p could not open SCM", name);
	
		if(!(service = OpenService(
				service_db,
				name,
				DELETE)))
			return_flail("w;f;p OpenService failed", name);

		su_free(name);
	
		if(!DeleteService(service))
			return_flail("w;;p DeleteService failed");
		
		if(!CloseServiceHandle(service))
			return_flail("w;;p could not close service");
	
		if(!CloseServiceHandle(service_db))
			return_flail("w;;p could not close SCM");

		if(su_log_uninstall(path))
			return_flail(";;p could not uninstall event log info");

		return_success();
#	else
		return_su_error(su_error_not_implemented);
#	endif
}
#endif
	
#if TARGET_WIN32
	static void WINAPI su_daemon_ctrl_handler(DWORD opcode)
	{
		switch(opcode)
		{
			case SERVICE_CONTROL_STOP:
				if(su_daemon_stopping())
				{	/* error has already been reported */
					/* what do we do here? */
					sds.error_code = 1;
				}
				if(sds.stop(sds.arg))
				{	/* well, set an error return code an hope we actually exit... */
					su_flail(";;rp stop function failed");
					sds.error_code = 1;
				}
			return;
			case SERVICE_CONTROL_INTERROGATE:
				/* report current status */
				if(!SetServiceStatus(sds.status_handle, &(sds.status)))
				{	/* wow we can't tell the service manager anything... */
					su_flail("w;;rp could not set service status");
					su_daemon_stopping();	/* try to say we're stopping */
					if(sds.stop(sds.arg))	/* stop daemon if possible */
						su_flail(";;rp stop function failed");
					sds.error_code = 1;
				}
			return;
			default:
				/* um. ok so what do we do here? */
				su_flail(";;rp unkown service opt");
				/* same song, second verse, doesn't get better, just gets worse */
				if(!SetServiceStatus(sds.status_handle, &(sds.status)))
				{	/* wow we can't tell the service manager anything... */
					su_flail("w;;rp could not set service status");
					su_daemon_stopping();	/* try to say we're stopping */
					if(sds.stop(sds.arg))	/* stop daemon if possible */
						su_flail(";;rp stop function failed");
					sds.error_code = 1;
				}
			return;
		}
	}

	static su_error_t su_daemon_stopped(unsigned int exit_code)
	{
		sds.status.dwCurrentState = SERVICE_STOPPED;
		sds.status.dwCheckPoint = 0;
		sds.status.dwWaitHint = 0;
		if(exit_code)
		{
			sds.status.dwServiceSpecificExitCode = exit_code;
			sds.status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
		}
		else if(sds.error_code)
		{
			sds.status.dwServiceSpecificExitCode = sds.error_code;
			sds.status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
		}
		else
			sds.status.dwWin32ExitCode = NO_ERROR;
		if(!SetServiceStatus(sds.status_handle, &(sds.status)))
			return_flail("w;;rp set service status error");
		return_success();
	}

	static su_error_t su_daemon_running(void)
	{
		sds.status.dwCurrentState = SERVICE_RUNNING;
		sds.status.dwCheckPoint = 0;
		sds.status.dwWaitHint = 0;
		if(!SetServiceStatus(sds.status_handle, &(sds.status)))
			return_flail("w;;rp set service status error");
		return_success();
	}

	static void su_daemon_status_init(void)
	{
		/* init counter to zero */
		sds.status_counter = 0;
	
		/* initialize status */
		sds.status.dwServiceType = SERVICE_WIN32;
		sds.status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		sds.status.dwWin32ExitCode = NO_ERROR;
		sds.status.dwServiceSpecificExitCode = 0;
		sds.status.dwCheckPoint = 0;
		sds.status.dwCurrentState = SERVICE_START_PENDING;
		sds.status.dwWaitHint = WAIT_HINT;
	}
	
#	if 0
	static unsigned long __stdcall su_daemon_run_thread(void *arg)
	{
		HANDLE *events = (HANDLE *)arg;
		/* events indexes */
		static const unsigned int go = 0;
		static const unsigned int done = 1;

		/* wait for go ahead to call init signal */
		switch(WaitForSingleObject(events[go], INFINITE))
		{
			case WAIT_ABANDONED:
			case WAIT_OBJECT_0:	/* got the event good */
				break;
			case WAIT_TIMEOUT:	/* how do you have a timeout on infinite? */
			case WAIT_FAILED:	/* error */
			default:		/* unknown */
				return_flail("w;;rp Failed to wait for go event");
		}

		/* call init */
		if(sds.init(sds.arg))
			return_error("Call to service init failed");

		/* inform controller that init is done */
		if(!SetEvent(events[done]))
			return_flail("w;;rp Could not send done event");

		/* wait for go ahead to call run */
		switch(WaitForSingleObject(events[go], INFINITE))
		{
			case WAIT_ABANDONED:
			case WAIT_OBJECT_0:	/* got the event good */
				break;
			case WAIT_TIMEOUT:	/* how do you have a timeout on infinite? */
			case WAIT_FAILED:	/* error */
			default:		/* unknown */
				return_flail("w;;rp Failed to wait for go event");
		}

		/* Execute daemon run function */
		if(sds.run(sds.arg))
			return_error("Call to service run failed");

		/* tell controller that service is done */
		if(!SetEvent(events[done]))
			return_flail("w;;rp Could not send done event");

		/* wait for go ahead to call cleanup */
		switch(WaitForSingleObject(events[go], INFINITE))
		{
			case WAIT_ABANDONED:
			case WAIT_OBJECT_0:	/* got the event good */
				break;
			case WAIT_TIMEOUT:	/* how do you have a timeout on infinite? */
			case WAIT_FAILED:	/* error */
			default:		/* unknown */
				return_flail("w;;rp Failed to wait for go event");
		}

		/* call cleanup */
		if(sds.cleanup(sds.arg))
			return_error("Call to service cleanup failed");

		/* we're done */
		if(!SetEvent(events[done]))
			return_flail("w;;rp Could not send done event");

		/* everything is done, end thread */
		return 0;
	}
#	endif
	
	static void WINAPI su_daemon_service_start(DWORD SU_CC_UNUSED_VAR argc, LPTSTR SU_CC_UNUSED_VAR *argv)
	{ 
		/* init status stuff */
		su_daemon_status_init();

		/* regiter ctrl handler function */
		if(!(sds.status_handle = RegisterServiceCtrlHandler(sds.name, su_daemon_ctrl_handler)))
		{	/* not much that we can do if this fails other than return */
			return_void_flail("w;;p register service ctrl handler failed");
		}
	 
		/* make sure that they know we're starting up */
		if(!su_daemon_starting())
		{	/* error will already have been reported */
			su_daemon_stopped(1);
			return;
		}

		/* call daemon init function */
		if(sds.init(sds.arg))
		{
			su_daemon_stopping();
			su_flail(";;rp error initting daemon");
			su_daemon_stopped(1);
			return;
		}

		/* tell SCM that we are running now */
		if(su_daemon_running())
		{	/* error will already have been reported */
			su_daemon_stopping();
			if(sds.cleanup(sds.arg))
				su_flail(";;rp failed to cleanup after error");
			su_daemon_stopped(1);
			return;
		}

		/* run main daemon */
		if(sds.run(sds.arg))
		{
			su_daemon_stopping();
			su_flail(";;rp error running daemon");
			if(sds.cleanup(sds.arg))
				su_flail(";;rp failed to cleanup after error");
			su_daemon_stopped(1);
			return;
		}

		/* stopping */
		if(!su_daemon_stopping())
		{	/* error has already been reported */
			su_daemon_stopping();
			if(sds.cleanup(sds.arg))
				su_flail(";;rp failed to cleanup after error");
			su_daemon_stopped(1);
			return;
		}

		/* run clean up code */
		if(sds.cleanup(sds.arg))
		{
			su_daemon_stopping();
			su_flail(";;rp error cleanning up daemon");
			su_daemon_stopped(1);
			return;
		}

		/* yeah, we are done! */
		if(su_daemon_stopped(0))
		{	/* can't do much more than return here */
			return_void_flail(";;p could not set stopped status");
		}
	
		/* out of here */
		return;
	}
#	if 0
	static void WINAPI su_daemon_service_start(DWORD argc, LPTSTR *argv)
	{ 
		HANDLE run_thread;
		DWORD wait_ret;
		static const unsigned int num_wait_handles = 4;
		HANDLE wait_handles[num_wait_handles];
		static const unsigned int run_thread_index	= 0;
		static const unsigned int done_event_index	= 1;
		static const unsigned int stop_event_index	= 2;
		static const unsigned int error_event_index	= 3;

		/* init counter to zero */
		sds.status_counter = 0;
	
		/* initialize status */
		sds.status.dwServiceType = SERVICE_WIN32;
		sds.status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		sds.status.dwWin32ExitCode = NO_ERROR;
		sds.status.dwServiceSpecificExitCode = 0;
		sds.status.dwCheckPoint = 0;
		sds.status.dwCurrentState = SERVICE_START_PENDING;
		sds.status.dwWaitHint = WAIT_HINT;

#		if 0
		/* others */
				/* if error, set this */
				sds.status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
				/* then this */
				sds.status.dwServiceSpecificExitCode = 0;
				/* time in milliseconds until estimated completion of pending event */
				sds.status.dwWaitHint = 0;
				/* should be incremented during pending event, otherwise 0 */
				sds.status.dwCheckPoint = 0;
#		endif

		/* need to make sure that error event is 0 */
		sds.error_event = (HANDLE)0;
	
		/* regiter ctrl handler function */
		if(!(sds.status_handle = RegisterServiceCtrlHandler(sds.name, su_daemon_ctrl_handler)))
			return_void_flail("w;;p register service ctrl handler failed");
	 
		/* make sure that they know we're starting up */
		if(!su_daemon_starting())
			return_void_flail("w;;p could not set starting status");

		/* create done event*/
		if(!(sds.done_event = CreateEvent(NULL, false, false, NULL)))
			return_void_flail("w;;p could not create event");

		/* make sure that they know we're starting up */
		if(!su_daemon_starting())
			return_void_flail("w;;p could not set starting status");

		/* create stop event */
		if(!(sds.stop_event = CreateEvent(NULL, false, false, NULL)))
			return_void_flail("w;;p could not create event");

		/* make sure that they know we're starting up */
		if(!su_daemon_starting())
			return_void_flail("w;;p could not set starting status");

		/* create go event */
		if(!(sds.go_event = CreateEvent(NULL, false, false, NULL)))
			return_void_flail("w;;p could not create event");

		/* make sure that they know we're starting up */
		if(!su_daemon_starting())
			return_void_flail("w;;p could not set starting status");

		/* create error event */
		if(!(sds.error_event = CreateEvent(NULL, false, false, NULL)))
			return_void_flail("w;;p could not create event");

		/* make sure that they know we're starting up */
		if(!su_daemon_starting())
			return_void_flail("w;;p could not set starting status");


		/* create run thread */
		if(!(run_thread = CreateThread(NULL, 0, su_daemon_run_thread, events, 0, &id)))
			return_void_flail("w;;p could not create run thread");

		/* still starting... */
		if(!su_daemon_starting())
			return_void_flail("w;;p could not set starting status");

		wait_handles[run_thread_index]	= run_thread;
		wait_handles[done_event_index]	= sds.done_event;
		wait_handles[stop_event_index]	= sds.stop_event;
		wait_handles[error_event_index]	= sds.error_event;

		/* still starting... */
		if(!su_daemon_starting())
			return_void_flail("w;;p could not set starting status");

		/* tell the run thread to start the init function */
		if(!SetEvent(events[go]))
			return_void_flail("w;;p could not set go status");

		/* wait for init function to finish and run thread to tell us */
		/* contnue updateing the SCM that we are starting */
		while((wait_ret = WaitForMultiplesObject(num_wait_handles, wait_handles, STATUS_INTERVAL) == WAIT_TIMEOUT))
		{	/* still starting... */
			if(!su_daemon_starting())
				return_void_flail("w;;p could not set starting status");
		}
		switch(wait_ret)
		{
			case WAIT_ABANDONED_0 + error_event_index:
			case WAIT_OBJECT_0 + error_event_index:
				/* error happened. do something */
			break;
			case WAIT_ABANDONED_0 + run_thread_index:
			case WAIT_OBJECT_0 + run_thread_index:
				/* run thread ended... */
				return_void_flail(";;p run thread ended unexpectedly");
			break;
			case WAIT_ABANDONED_0 + stop_event_index:
			case WAIT_OBJECT_0 + stop_event_index:
				/* not even started and we're being stopped */
			break;
			case WAIT_ABANDONED_0 + done_event_index:
			case WAIT_OBJECT_0 + done_event_index:
				/* this is what we want, no error */
			break;
			case WAIT_TIMEOUT:	/* shouldn't happen, loop wouldn't have stopped */
			case WAIT_FAILED:	/* error */
			default:		/* unknown */
				return_flail("w;;rp Error occured while waiting  for init to finish");
		}

		/* not quite started */
		if(!su_daemon_starting())
			return_void_flail("w;;p could not set starting status");
	
		/* tell run thread to start the main part */
		SetEvent(events[go]);

		/* tell SCM that we are running now */
		sds.status.dwCurrentState = SERVICE_RUNNING;
		sds.status.dwCheckPoint = 0;
		sds.status.dwWaitHint = 0;
		if(!SetServiceStatus(sds.status_handle, &(sds.status)))
			return_void_flail("w;;p set service status error");

		/* wait for daemon to be done */
		switch(WaitForMultiplesObject(num_wait_handles, wait_handles, INFINITE))
		{
			case WAIT_ABANDONED_0 + error_event_index:
			case WAIT_OBJECT_0 + error_event_index:
				/* error happened. do something */
			break;
			case WAIT_ABANDONED_0 + run_thread_index:
			case WAIT_OBJECT_0 + run_thread_index:
				/* run thread ended... */
				return_void_flail(";;p run thread ended unexpectedly");
			break;
			case WAIT_ABANDONED_0 + stop_event_index:
			case WAIT_OBJECT_0 + stop_event_index:
				/* not even started and we're being stopped */
			break;
			case WAIT_ABANDONED_0 + done_event_index:
			case WAIT_OBJECT_0 + done_event_index:
				/* this is what we want, no error */
			break;
			case WAIT_TIMEOUT:	/* shouldn't happen, loop wouldn't have stopped */
			case WAIT_FAILED:	/* error */
			default:		/* unknown */
				return_flail("w;;rp Error occured while waiting for run to finish");
		}

		/* stopping */
		sds.status.dwCheckPoint = 0;
		if(!su_daemon_stopping())
			return_void_flail("w;;p could not set stopping status");

		/* tell run thread to start the cleanup */
		SetEvent(events[go]);

		/* stopping */
		sds.status.dwCheckPoint = 0;
		if(!su_daemon_stopping())
			return_void_flail("w;;p could not set stopping status");

		/* wait for cleanup function to finish and run thread to tell us */
		/* contnue updateing the SCM that we are stopping */
		while((wait_ret = WaitForSingleObject(sds.event_done, STATUS_INTERVAL) == WAIT_TIMEOUT))
		{	/* still stopping... */
			if(!su_daemon_stopping())
				return_void_flail("w;;p could not set stopping status");
		}
		switch(wait_ret)
		{
			case WAIT_ABANDONED:
			case WAIT_OBJECT_0:	/* got the event good */
				break;
			case WAIT_TIMEOUT:	/* shouldn't happen, loop wouldn't have stopped */
			case WAIT_FAILED:	/* error */
			default:		/* unknown */
				return_flail("w;;rp Failed to wait for go event");
		}
	
		/* still stopping */
		if(!su_daemon_stopping())
			return_void_flail("w;;p could not set stopping status");

		/* wait for run thread to end */
		/* contnue updateing the SCM that we are waiting */
		while((wait_ret = WaitForSingleObject(run_thread, STATUS_INTERVAL) == WAIT_TIMEOUT))
		{	/* still stopping... */
			if(!su_daemon_stopping())
				return_void_flail("w;;p could not set stopping status");
		}
		switch(wait_ret)
		{
			case WAIT_ABANDONED:
			case WAIT_OBJECT_0:	/* got the event good */
				break;
			case WAIT_TIMEOUT:	/* shouldn't happen, loop wouldn't have stopped */
			case WAIT_FAILED:	/* error */
			default:		/* unknown */
				return_flail("w;;rp Failed to wait for go event");
		}
	
		/* yeah, we are done! */
		sds.status.dwCurrentState = SERVICE_STOPPED;
		sds.status.dwCheckPoint = 0;
		sds.status.dwWaitHint = 0;
		sds.status.dwWin32ExitCode = 0;
		if(!SetServiceStatus(sds.status_handle, &(sds.status)))
		{
			errormessage("could not set service status");
			exit(1);
		}
	
		return;

		/* on error:
		 * set status to stopping
		 * if running, send stop signal
		 * kill threads
		 * join threads
		 * set Win32ExitCode
		 * set Specific exit code
		 * set stopped
		 * return
		 */
	}
#	endif

#elif TARGET_UNIX

	void su_daemon_sig_term(int SU_CC_UNUSED_VAR n)
	{
		sds.stop(sds.arg);
	}

#endif
	
su_error_t su_daemon_starting(void)
{
#	if TARGET_WIN32
		sds.status_counter++;
		sds.status.dwCurrentState = SERVICE_START_PENDING;
		sds.status.dwCheckPoint = sds.status_counter;
		sds.status.dwWaitHint = WAIT_HINT;
		if(!SetServiceStatus(sds.status_handle, &(sds.status)))
			return_flail("w;;rp set service status returned error");
#	endif
	return_success();
}
	
su_error_t su_daemon_stopping(void)
{
#	if TARGET_WIN32
		sds.status_counter++;
		sds.status.dwCurrentState = SERVICE_STOP_PENDING;
		sds.status.dwCheckPoint = sds.status_counter;
		sds.status.dwWaitHint = WAIT_HINT; 
		if(!SetServiceStatus(sds.status_handle, &(sds.status)))
			return_flail("w;;rp set service status error");
#	endif
	return_success();
}
	
/** Main method for daemon.
 *
 * This handles all the necessary requirements for the system to create a 
 * daemonized program. This is sometimes refered to as a service or a program
 * that runs in the background.
 *
 * This should be called as the only function in your program's main. It does
 * not return. Note that your program should have a main that just runs this
 * function. This keeps a main function out of the library which would cause
 * all sorts of other problems.
 *
 * Name is a charachter string that is the name of the daemon. This can be 
 * destinct from the path of the program.
 *
 * Path is the path to the program binary. This is necessary for systems
 * where this is needed by a service manager (ie windows).
 *
 * All startup initialization should be done in the init function passed. No 
 * intialization and such should be run before calling su_daemon. This is 
 * important because of the way some systems (ie windows) handle daemons 
 * (or as windows calls them: services). This function can be NULL if your
 * daemon requires NO initialization. The function is passed arg as it's one 
 * paramater.
 * 
 * The main daemon loop should be in the run function. This is where the deamon
 * should do its work. It is called once after the init function. If it returns
 * without the stop function being called first, it is considered an error and 
 * an error will be returned to the system. This function can be NULL but will
 * result in an error. The function is passed arg as it's one paramater.
 *
 * The stop function should stop the daemon. This function is special in that
 * it will be called asyncronously and possibly in another thread from the one
 * that your run function is called (ie the run function may still be executing
 * when this function is called if the system implementation necessitates it
 * like it does in windows). Note that your stop function should only signal 
 * the run function to stop and return quickly. The run function should reserve
 * cleanup for the cleanup function. The run function must return before cleanup
 * will be called. As other functions, arg is passed as it's one paramater.
 *
 * The cleanup function is called after the run function has finished. This 
 * function should take care of any cleanup tasks that are necessary. When it
 * finishes, the system is notified of the daemon being stopped. As usual, arg
 * is passwd as it's one paramater.
 *
 * argc and argv are the argc and argv that are passed to main. 
 *
 * An example of a correct main would be as follows:
 *
 * int main(int argc, char **argv)
 * {
 * 	su_daemon("my daemon", "/usr/sbin/mydaemon", my_init, my_run, my_stop,
 * 		my_cleanup, my_arg, argc, argv);
 * 	return 0;
 * }
 *
 * The return 0 is to make compilers that check that functions that return a
 * value actually do return something. It is not run as su_daemon never returns
 * and exits the program with exit() (or it's equivelent).
 */
void su_daemon(const char *name, const su_daemon_callback init, const su_daemon_callback run, const su_daemon_callback stop, const su_daemon_callback cleanup, void *arg)
{
	sds.init	= init;
	sds.run		= run;
	sds.stop	= stop;
	sds.cleanup	= cleanup;
	sds.arg		= arg;
	sds.name 	= name;
#	if TARGET_WIN32
		SERVICE_TABLE_ENTRY DispatchTable[] =
		{
			{(char *)sds.name, su_daemon_service_start},
			{(LPSTR)NULL,(LPSERVICE_MAIN_FUNCTION)NULL}
		};
#	endif

	/* start logging stuff */
	if(su_log_init_syslog(name, su_log_daemon))
		su_fatal_flail(1, ";;rp failed to initialize logging");

#	if TARGET_WIN32
		if(!StartServiceCtrlDispatcher(DispatchTable))
			su_fatal_flail(1, "w;;rp could not start service dispatcher");
#	elif TARGET_UNIX
		/* call the init routine */
		if(sds.init(sds.arg))
			exit(1);

		/* set signal handler for SIGTERM to call the stop function */
		if(signal(SIGTERM, su_daemon_sig_term) == SIG_ERR)
			su_fatal_flail(1, "sS;;r", su_error_install_sig_handler, "setting SIGTERM");

		/* might as well handle user stop as well */
		if(signal(SIGINT, su_daemon_sig_term) == SIG_ERR)
			su_fatal_flail(1, "sS;;rx", su_error_install_sig_handler, "setting SIGINT");

		/* call daemon to fork us off and such */
#		if HAVE_DAEMON
			if(daemon(0,0))
				su_fatal_flail(1, "sS;;r", su_error_fork, "could not daemonize process");
#		else
#			error we require daemon right now
#		endif

		/* call the run function */
		if(sds.run(sds.arg))
			exit(1);

		if(sds.cleanup(sds.arg))
			exit(1);
#	endif

	exit(0);
}

// HERE
#if 0
int main(int argc, char **argv)
{
	A_daemon_init();
	// startup code here
	A_daemon();
	// daemon loop, etc
	// ...gets stop callback...
	// cleanup code
	A_daemon_cleanup();
}
#endif

static A_daemon_stop_func_t stop_func = NULL;

#if TARGET_WIN32
	static SERVICE_STATUS		status;
	static SERVICE_STATUS_HANDLE	status_handle;
	static DWORD			status_counter;
	static unsigned int 		error_code;
	static const char*		service_name = NULL;

	static w32_ctrl_handler()
	{
	}

	static void w32_status_init(void)
	{
		/* init counter to zero */
		status_counter = 0;
	
		/* initialize status */
		status.dwServiceType = SERVICE_WIN32;
		status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		status.dwWin32ExitCode = NO_ERROR;
		status.dwServiceSpecificExitCode = 0;
		status.dwCheckPoint = 0;
		status.dwCurrentState = SERVICE_START_PENDING;
		status.dwWaitHint = WAIT_HINT;
	}

	static void WINAPI w32_service_start(DWORD SU_CC_UNUSED_VAR argc, LPTSTR SU_CC_UNUSED_VAR *argv)
	{
		w32_status_init();

		/* regiter ctrl handler function */
		if(!(sds.status_handle = RegisterServiceCtrlHandler(service_name, w32_ctrl_handler)))
		{	/* not much that we can do if this fails other than return */
			return_void_flail("w;;p register service ctrl handler failed");
		}
	}
#endif

A_daemon_init(const char *name, A_daemon_stop_func_t stop_func_n)
{
	stop_func = stop_func_n;
	service_name = name;

#	if TARGET_WIN32
		SERVICE_TABLE_ENTRY DispatchTable[] =
		{
			{name, w32_service_start},
			{(LPSTR)NULL,(LPSERVICE_MAIN_FUNCTION)NULL}
		};

		if(!StartServiceCtrlDispatcher(DispatchTable))
			die_win32("could not start service dispatcher");

		// we're done now in w32 land
		exit(0);
#	endif

}

A_daemon()
{
}

A_daemon_cleanup()
{
}
