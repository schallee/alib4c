# DM_OS_TYPE
# ------------------------------------------------------------------------------
AC_DEFUN([DM_OS_TYPE],
[
	AC_REQUIRE([AC_CANONICAL_HOST])
	AC_CACHE_CHECK([for host operating system type], [dm_cv_os_type],
	[
		case "$host_os" in
			*mingw*|*windows*)
				dm_cv_os_type=win32
				;;
			*unix*|*linux*|*bsd*|*cygwin*|"darwin")
				dm_cv_os_type=unix
				;;
			*)
				dm_cv_os_type="unknown (assuming unix)"
				;;
		esac
	])
	AS_IF([test x"$dm_cv_os_type" = x"unknown (assuming unix)"], [dm_cv_os_type=unix])
	AC_DEFINE_UNQUOTED([OS_TYPE],"$dm_cv_os_type",[define to the operating system type])
])dnl

# DM_OS_TYPE_WIN32([ACTION-IF-TRUE], [ACTION-IF-NOT-TRUE])
# ------------------------------------------------------------------------------
AC_DEFUN([DM_OS_TYPE_WIN32],
[
	AC_REQUIRE([DM_OS_TYPE])
	AS_IF([test x"$dm_cv_os_type" = x"win32"],
	[
		m4_if([$1],[],
		[
			AC_DEFINE([OS_TYPE_WIN32],[1],[define to 1 if the os type is win32])
		],[$1])
	],[
		m4_if([$2],[],[:],[$2])
	])
])dnl

# DM_OS_TYPE_UNIX([ACTION-IF-TRUE], [ACTION-IF-NOT-TRUE])
# ------------------------------------------------------------------------------
AC_DEFUN([DM_OS_TYPE_UNIX],
[
	AC_REQUIRE([DM_OS_TYPE])
	AS_IF([test x"$dm_cv_os_type" = x"unix"],
	[
		m4_if([$1],[],
		[
			AC_DEFINE([OS_TYPE_UNIX],[1],[define to 1 if the os type is unix])
		],[$1])
	],[
		m4_if([$2],[],[:],[$2])
	])
])dnl
