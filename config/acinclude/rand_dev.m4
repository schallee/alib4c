# DM_DEV_URANDOM([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [DEFAULT-IF-CROSS-COMPILING])
# ------------------------------------------------------------------------------
AC_DEFUN([DM_DEV_URANDOM],
[
	AC_REQUIRE([DM_OS_TYPE])
	AC_CACHE_CHECK([for /dev/urandom], [dm_cv_sys_dev_urandom_exists],
	[
		DM_OS_TYPE_UNIX(
		[
			AS_IF([test x"$cross_compiling" = x"yes"],
			[
				m4_if([$3],[],
				[
					dm_cv_sys_dev_urandom_exists="unknown (cross compiling)"
				],[
					dm_cv_sys_dev_urandom_exists=$3
				])
			],[
				dm_cv_sys_dev_urandom_exists=no
				test -c /dev/urandom && dm_cv_sys_dev_urandom_exists=yes
			])
		],[
			dm_cv_sys_dev_urandom_exists="no (os type not unix)"
		])
	])
	AS_IF([test x"$dm_cv_sys_dev_urandom_exists" = x"yes"],
	[
		m4_if([$1],[],
		[
			AC_DEFINE([HAVE_DEV_URANDOM], 1, [Define to 1 if your system has /dev/urandom])
		],[$1])
	],[
		m4_if([$2],[],[:],[$2])
	])
])dnl

# DM_DEV_RANDOM([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [DEFAULT-IF-CROSS-COMPILING])
# ------------------------------------------------------------------------------
AC_DEFUN([DM_DEV_RANDOM],
[
	AC_REQUIRE([DM_OS_TYPE])
	AC_CACHE_CHECK([for /dev/random], [dm_cv_sys_dev_random_exists],
	[
		DM_OS_TYPE_UNIX(
		[
			AS_IF([test x"$cross_compiling" = x"yes"],
			[
				m4_if([$3],[],
				[
					dm_cv_sys_dev_random_exists="unknown (cross compiling)"
				],[
					dm_cv_sys_dev_random_exists=[$3]
				])
			],[
				dm_cv_sys_dev_random_exists=no
				test -c /dev/random && dm_cv_sys_dev_random_exists=yes
			])
		],[
			dm_cv_sys_dev_random_exists="no (os type not unix)"
		])
	])
	AS_IF([test x"$dm_cv_sys_dev_random_exists" = x"yes"],
	[
		m4_if([$1],[],
		[
			AC_DEFINE([HAVE_DEV_RANDOM], 1, [Define to 1 if your system has /dev/random])
		],[$1])
	],[
		m4_if([$2],[],[:],[$2])
	])
])dnl

# DM_RAND_DEV([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ------------------------------------------------------------------------------
AC_DEFUN([DM_RAND_DEV],
[
	AC_REQUIRE([DM_DEV_URANDOM])
	AC_REQUIRE([DM_DEV_RANDOM])
	AC_CACHE_CHECK([to see if we found a random device], [dm_cv_sys_rand_dev_exists],
	[
		AS_IF([test x"$dm_cv_sys_dev_urandom_exists" = x"yes" || test x"$dm_cv_sys_dev_random_exists" = x"yes"],
		[
			dm_cv_sys_rand_dev_exists="yes"
		],[
			dm_cv_sys_rand_dev_exists="no"
		])
	])
	AS_IF([test x"$dm_cv_sys_rand_dev_exists" = x"yes"],
	[
		AC_CACHE_CHECK([for random device to use], [dm_cv_sys_rand_dev],
		[
			AS_IF([test x"$dm_cv_sys_dev_urandom_exists" = x"yes"],
			[
				dm_cv_sys_rand_dev="/dev/urandom"
			],[
				dm_cv_sys_rand_dev="/dev/random"
			])
		])
		m4_if([$1],[],
		[
			AC_DEFINE([HAVE_RAND_DEV],[1],[Define to 1 if the system has a random device])
			AC_DEFINE_UNQUOTED([RAND_DEV],"$dm_cv_sys_rand_dev",[Define to the random device to use])
		],[$1])
	],[
		m4_if([$2],[],[:],[$2])
	])
])dnl
