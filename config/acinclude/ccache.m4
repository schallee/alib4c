# DM_PROG_CCACHE
# --------------
AC_DEFUN([DM_PROG_CCACHE],
[
	AC_REQUIRE([AC_PROG_CC])
	dnl did they pass their own CC?
	AS_IF([test x"$ac_env_cc_set" != x"yes"],
	[
		AC_CHECK_PROG([ac_cv_prog_ccache], [ccache], [yes], [no])
		AS_IF([test x"$ac_cv_prog_ccache" = x"yes"],
		[
			CC="ccache $CC"
		])
	])
])
