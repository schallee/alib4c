AC_INIT([a],[0.1.0],[schallee@darkmist.net])
AC_PREREQ(2.59)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config/acinclude)
AC_CONFIG_HEADERS(config/config.h)
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE

DM_OS_TYPE
DM_OS_TYPE_WIN32
DM_OS_TYPE_UNIX

# are we targeting windows?
DM_OS_TYPE_WIN32(
[
	AC_DEFINE([TARGET_WIN32],[1],[define to 1 if the os type is win32])
	AC_LIBTOOL_RC
	AC_LIBTOOL_WIN32_DLL
	:	# we need something here as above seems to be NULL now...
],[
	AC_DEFINE([TARGET_UNIX],[1],[define to 1 if the os type is unix])
])
AC_LIBTOOL_TAGS([RC])
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC
DM_PROG_CCACHE

# these are all from autoscan
#AC_PROG_CXX	# autoscan says libtool still needs this....
#AC_PROG_CPP
#AC_PROG_INSTALL
#AC_PROG_LN_S
#AC_PROG_MAKE_SET
#AC_PROG_RANLIB

# C features
AC_C_PROTOTYPES
AM_C_PROTOTYPES
AC_C_CONST
AC_LANG_WERROR
AC_C_INLINE
AC_C_STRINGIZE
AC_C_BIGENDIAN
AC_SYS_LARGEFILE

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AS_IF([test x"$ac_cv_header_stdbool_h" = x"yes"],
[
	AC_SUBST([HEADER_STDBOOL_H], ["#include <stdbool.h>"])
],[
	AS_IF([test x"$ac_cv_type__Bool" = x"yes"],
	[
		HEADER_STDBOOL_H="$srcdir/config/include/last_chance_stdbool_with_bool.h"
	],[
		HEADER_STDBOOL_H="$srcdir/config/include/last_chance_stdbool.h"
	])
	AC_SUBST_FILE([HEADER_STDBOOL_H])
])
AC_CHECK_HEADERS([inttypes.h stdint.h], [break])
AS_IF([test x"$ac_cv_header_stdint_h" = x"yes"],
[
	AC_SUBST([HEADER_STDINT_H], ["#include <stdint.h>"])
],[
	AS_IF([test x"$ac_cv_header_inttypes_h" = x"yes"],
	[
		AC_SUBST([HEADER_STDINT_H], ["#include <inttypes.h>"])
	],[
		HEADER_STDINT_H="$srcdir/config/include/last_chance_stdint.h"
		AC_SUBST_FILE([HEADER_STDINT_H])
		AC_MSG_WARN([Didnt' find stdint.h or inttypes. Will try some standard ones.])
	])
])
AC_CHECK_HEADERS([stdarg.h])
AS_IF([test x"$ac_cv_header_stdarg_h" = x"yes"],
[
	AC_SUBST([HAVE_STDARG_H], [1])
],[
	AC_MSG_ERROR([We currently require stdarg.h])
	dnl AC_SUBST([HAVE_STDARG_H], [0])
])
AC_CHECK_HEADERS([stdlib.h])
DM_YES_SUBST_1([$ac_cv_header_stdlib_h], [HAVE_STDLIB_H])
AC_CHECK_HEADERS([stddef.h])
DM_YES_SUBST_1([$ac_cv_header_stddef_h], [HAVE_STDDEF_H])
AC_CHECK_HEADERS([byteswap.h])
DM_YES_SUBST_1([$ac_cv_header_byteswap_h], [HAVE_BYTESWAP_H])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([sys/types.h])
DM_YES_SUBST_1([$ac_cv_header_sys_types_h], [HAVE_SYS_TYPES_H])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([sys/file.h])
AC_CHECK_HEADERS([signal.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([sys/stat.h])
AC_CHECK_HEADERS([ctype.h])
DM_OS_TYPE_WIN32(
[
	AC_SUBST([OS_TYPE_WIN32], [1])
	AC_SUBST([HAVE_NETDB_H], [0])
	AC_SUBST([HAVE_NETINET_IN_H], [0])
	DM_HEADER_WINDOWS_H
	DM_YES_SUBST_1([$ac_cv_header_windows_h], [HAVE_WINDOWS_H])
	DM_HEADER_WINSOCK2_H
	DM_YES_SUBST_1([$ac_cv_header_winsock2_h], [HAVE_WINSOCK2_H])
],[
	AC_SUBST([OS_TYPE_WIN32], [0])
	AC_SUBST([HAVE_WINDOWS_H], [0])
	AC_SUBST([HAVE_WINSOCK2_H], [0])
	AC_CHECK_HEADERS([arpa/inet.h])
	AC_CHECK_HEADERS([netdb.h])
	DM_YES_SUBST_1([$ac_cv_header_netdb_h], [HAVE_NETDB_H])
	AC_CHECK_HEADERS([arpa/inet.h])
	AC_CHECK_HEADERS([netinet/in.h])
	DM_YES_SUBST_1([$ac_cv_header_netinet_in_h], [HAVE_NETINET_IN_H])
	AC_CHECK_HEADERS([sys/socket.h])
	AC_CHECK_HEADERS([syslog.h])
	AC_CHECK_HEADERS([sys/mman.h])
])
AC_CHECK_HEADERS([openssl/err.h])
DM_YES_SUBST_1([$ac_cv_header_openssl_err_h], [HAVE_OPENSSL_ERR_H])
AM_CONDITIONAL(HAVE_OPENSSL, test x"$ac_cv_header_openssl_err_h" = x"yes")

# Checks for typedefs, structures, and compiler characteristics.
# for library
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_CHECK_TYPES(socklen_t)

# Checks for libraries.
DM_OS_TYPE_WIN32(
[
	DM_LIB_WS2_32([], [AC_MSG_ERROR([winsock2 is required for win32 targets])])
])

# Checks for library functions.
AC_CHECK_FUNCS([bswap_64])
AC_CHECK_FUNCS([inet_pton inet_aton inet_ntoa],[break])
AC_CHECK_FUNCS([inet_ntop])
AC_CHECK_FUNCS([alarm])
AC_CHECK_FUNCS([gethostbyaddr])
AC_CHECK_FUNCS([gethostbyname])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([munmap])
AC_CHECK_FUNCS([socket])
AC_CHECK_FUNCS([strerror_r strerror],[break])
AC_CHECK_FUNCS([strrchr])
AC_CHECK_FUNCS([strtoul])
AC_CHECK_FUNCS([close])
AC_CHECK_FUNCS([unlink])
AC_CHECK_FUNCS([isblank])
AC_CHECK_FUNCS([isspace])
AC_CHECK_FUNCS([vsyslog])
AC_CHECK_FUNCS([hstrerror])
AC_CHECK_FUNCS([fsync])
#AC_FUNC_MALLOC
AC_FUNC_ALLOCA
ac_cv_header_alloca_h=$ac_cv_working_alloca_h
DM_YES_SUBST_1([$ac_cv_header_alloca_h], [HAVE_ALLOCA_H])
#AC_FUNC_MEMCMP
DM_OS_TYPE_WIN32(
[
	AC_CHECK_FUNCS([DeleteFile])
	AC_CHECK_FUNCS([CloseHandle])
],[
	AC_CHECK_FUNCS([daemon], [], [AC_MSG_ERROR([currently require daemon, this will be fixed])])
	AC_FUNC_MMAP
])

# Checks for declarations
AC_CHECK_DECLS([errno], [], [],
[
#	if HAVE_ERRNO_H
#		include <errno.h>
#	endif
])
AC_CHECK_DECLS([isblank], [], [],
[
#	if HAVE_CTYPE_H
#		include <ctype.h>
#	endif
])
AC_CHECK_DECLS([isspace], [], [],
[
#	if HAVE_CTYPE_H
#		include <ctype.h>
#	endif
])
AC_CHECK_DECLS([vsyslog], [], [],
[
#	if HAVE_SYSLOG_H
#		include <syslog.h>
#	endif
#	if HAVE_STDARG_H
#		include <stdarg.h>
#	endif
])
AC_CHECK_DECLS([h_errno], [],[],
[
#	if HAVE_NETDB_H
#		include <netdb.h>
#	endif
])
AC_CHECK_DECLS([daemon], [], [],
[
#	if HAVE_STDLIB_H
#		include <stdlib.h>
#	endif
])
AC_CHECK_DECLS([fsync], [], [],
[
#	if HAVE_UNISTD_H
#		include <unistd.h>
#	endif
])

# Checks for system functionality

# automake conditionals
AM_CONDITIONAL(TARGET_WIN32, test x"$dm_cv_os_type" = x"win32")
AM_CONDITIONAL(TARGET_UNIX, test x"$dm_cv_os_type" = x"unix")

# options
AM_WITH_DMALLOC
AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug], [enable debuging])])
AM_CONDITIONAL([DEBUG], [test x"$enable_debug" = x"yes"])

AC_ARG_ENABLE([mass-warnings], [AS_HELP_STRING([--enable-mass-warnings], [enable massive numbers of compiler warnings as errors])])
AM_CONDITIONAL([MASS_WARNINGS], [test x"$enable_mass_warnings" = x"yes"])
AS_IF([test x"$enable_mass_warnings" = x"yes"],[AC_DEFINE([MASS_WARNINGS],[1],[define to 1 if you are compiling with mass warings])])

AC_ARG_ENABLE([verbose-tests], [AS_HELP_STRING([--enable-verbose-tests], [enable verbose testing for make check])])
AM_CONDITIONAL([USE_VERBOSE_TESTS], [test x"$enable_verbose_tests" = x"yes"])
AS_IF([test x"$enable_verbose_tests" = x"yes"],[AC_DEFINE([USE_VERBOSE_TESTS],[1],[define to 1 if you are compiling with verbose testing for make check])])

# Output 
#LTLIBOBJS=`echo "$LIBOBJS" | sed 's/\.[^.]* /.lo /g;s/\.[^.]*$/.lo/'`
#AC_SUBST(LTLIBOBJS)
#LTALLOCA=`echo "$ALLOCA" | sed 's/\.[^.]* /.lo /g;s/\.[^.]*$/.lo/'`
#AC_SUBST(LTALLOCA)
#set > config.vars
AC_CONFIG_FILES([a/resolve.h a/clean.h a/error.h a/error_mod.h a/types.h a/varg.h a/util.h a/alloca.h a/Makefile test/Makefile test/test_common/Makefile test/util/Makefile Makefile])
AC_OUTPUT
