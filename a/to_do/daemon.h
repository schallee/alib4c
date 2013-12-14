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
