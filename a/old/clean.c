#if HAVE_CONFIG_H
#	include <config.h>
#endif

#include <stdio.h>

#include <a/clean_mod.h>
#include <a/error.h>
#include <a/varg.h>

#if HAVE_UNISTD_H
#	include <unistd.h>
#endif
#if HAVE_STDLIB_H
#	include <stdlib.h>
#endif
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_MMAN_H
#	include <sys/mman.h>
#endif

/*************
 * functions *
 *************/

extern a_status_t a_clean_vargs(const a_clean_mod_t *mod, va_list *args)
{
	a_status_t ret = 0;

	while(mod)
	{
		ret |= mod->func(args);
		mod = va_arg(*args, const a_clean_mod_t *);
	}
	return ret;
}

extern a_status_t a_clean_real(const a_clean_mod_t *mod, ...)
{
	va_list args;
	a_status_t ret;

	va_start(args, mod);
	ret = a_clean_vargs(mod, &args);
	va_end(args);
	return ret;
}

/***********
 * modules *
 ***********/

a_clean_mod_decl_m(socket)
{
	a_socket_t sock;

	sock = va_arg(*args, a_socket_t);
	if(a_socket_invalid_m(sock))
		return A_STATUS_GOOD;
#	if !OS_TYPE_WIN32 && HAVE_CLOSE
		if(!close(sock))
			return A_STATUS_SUCCESS;
		a_error_add_m(a_error_posix);
		return A_STATUS_ERROR;
#	else
		a_error_add_m(a_error_lib, A_ERROR_NOT_IMPLEMENTED);
		return A_STATUS_ERROR;
#	endif
}

a_clean_mod_decl_m(fd)
{
	a_fd_t fd;

	fd = va_arg(*args, a_fd_t);
	if(a_fd_invalid_m(fd))
		return A_STATUS_SUCCESS;
#	if !OS_TYPE_WIN32 && HAVE_CLOSE
		if(!close(fd))
			return A_STATUS_SUCCESS;
		a_error_add_m(a_error_posix);
		return A_STATUS_ERROR;
#	else
		a_error_add_m(a_error_lib, A_ERROR_NOT_IMPLEMENTED);
		return A_STATUS_ERROR;
#	endif
}

a_clean_mod_decl_m(file_ptr)
{
	FILE *file;

	file = va_arg(*args, FILE *);
	if(!file)
		return A_STATUS_SUCCESS;
	if(!fclose(file))
		return A_STATUS_SUCCESS;
	a_error_add_m(a_error_stdc);
	return A_STATUS_ERROR;
}

#if OS_TYPE_WIN32 && HAVE_WINDOWS_H
	a_clean_mod_decl_m(handle)
	{
		HANDLE handle;
	
		handle = va_arg(*args, HANDLE);
		if(handle == INVALID_HANDLE_VALUE)
			return A_STATUS_SUCCESS;
		if(CloseHandle(handle))
			return A_STATUS_SUCCESS;
		a_error_add_m(a_error_win);
		return A_STATUS_ERROR;
	}
#endif

a_clean_mod_decl_m(free)
{
	void *ptr;

	ptr = va_arg(*args, void *);
	if(ptr)
		free(ptr);
	return A_STATUS_SUCCESS;
}

a_clean_mod_decl_m(unmap)
{
	void *ptr;
	size_t size;

	ptr = va_arg(*args, void *);
	size = va_arg(*args, size_t);
	if(!ptr || !size)
		return A_STATUS_SUCCESS;
#	if HAVE_MMAP
		if(!munmap(ptr, size))
			return A_STATUS_SUCCESS;
		a_error_add_m(a_error_posix);
		return A_STATUS_ERROR;
#	else
		a_error_add_m(a_error_lib, A_ERROR_NOT_IMPLEMENTED);
		return A_STATUS_ERROR;
#	endif
}

a_clean_mod_decl_m(unlink)
{
	char *file;

	file = va_arg(*args, char *);
#	if !OS_TYPE_WIN32 && HAVE_UNLINK
		if(!unlink(file))
			return A_STATUS_SUCCESS;
		a_error_add_m(a_error_posix);
		return A_STATUS_ERROR;
#	elif OS_TYPE_WIN32
		if(DeleteFile(file))
			return A_STATUS_SUCCESS;
		a_error_add_m(a_error_win);
		return A_STATUS_ERROR;
#	else
		a_error_add_m(a_error_lib, A_ERROR_NOT_IMPLEMENTED);
		return A_STATUS_ERROR;
#	endif
}
