# DM_HEADER_WINDOWS_H([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
#
# check for windows.h
AC_DEFUN([DM_HEADER_WINDOWS_H],[AC_CHECK_HEADERS([windows.h], $1, $2)])dnl

# DM_HEADER_WINSOCK2_H([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
#
# checks for winsock2.h
AC_DEFUN([DM_HEADER_WINSOCK2_H],[AC_CHECK_HEADERS([winsock2.h],$1,$2)])dnl

# DM_HEADER_WINCRYPT_H([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
#
# checks for wincrypt.h
AC_DEFUN([DM_HEADER_WINCRYPT_H],[
	AC_REQUIRE([DM_HEADER_WINDOWS_H])dnl
	AC_CHECK_HEADERS([wincrypt.h],$1,$2,[
#		if HAVE_WINDOWS_H
#			include<windows.h>
#		endif
	])
])dnl

# DM_LIB_WINSOCK2([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
#
# Check linking with winsock2 library (ws2_32.dll). If it is found and 
# ACTION-IF-FUOND is not specified, HAVE_LIBWS2_32 is defined and -lws2_32 is
# added to LIBS.
AC_DEFUN([DM_LIB_WS2_32],[
	AC_REQUIRE([DM_HEADER_WINDOWS_H])dnl
	AC_REQUIRE([DM_HEADER_WINSOCK2_H])dnl
	AC_CACHE_CHECK([for ws2_32.dll], [dm_cv_lib_ws2_32],[
		AC_LANG_PUSH(C)
		dm_tmp_libs="$LIBS"
		LIBS="$LIBS -lws2_32"
		AC_LINK_IFELSE(AC_LANG_SOURCE([[
#			if HAVE_WINDOWS_H
#				include <windows.h>
#			endif
#			if HAVE_WINSOCK2_H
#				include <winsock2.h>
#			endif

			int main()
			{
				WSADATA wsadata;
				int ret;
			
				/* try calling WSAStartup */
				if(!(ret = WSAStartup(MAKEWORD(2,0), &wsadata)))
					/* clean it up if it worked */
					WSACleanup();
			
				/* return the result */
				exit(ret);
			}
		]]), [dm_cv_lib_ws2_32=yes], [dm_cv_lib_ws2_32=no])
		LIBS="$dm_tmp_libs"
		AC_LANG_POP(C)
	])
	AS_IF([test x"$dm_cv_lib_ws2_32" = x"yes"],[
		m4_if([$1], [], [
			AC_DEFINE(HAVE_LIBWS2_32, 1, [define to 1 if your system has ws2_32.dll])
			LIBS="$LIBS -lws2_32"
		],[$1])
	],[
		m4_if([$2], [], [:], [$2])
	])
])dnl

# DM_LIB_ADVAPI32([ACTION-IF-FOUND[, [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------------------
#
# Check linking with advapi library (advapi32.dll). If it is found and 
# ACTION-IF-FUOND is not specified, HAVE_LIBADVAPI32 is defined and -ladvapi32 is
# added to LIBS.
AC_DEFUN([DM_LIB_ADVAPI32],[
	AC_REQUIRE([DM_HEADER_WINDOWS_H])dnl
	AC_REQUIRE([DM_HEADER_WINCRYPT_H])dnl
	AC_CACHE_CHECK([for advapi32.dll], [dm_cv_lib_advapi32],[
		AC_LANG_PUSH(C)
		dm_tmp_libs="$LIBS"
		LIBS="$LIBS -ladvapi32"
		AC_LINK_IFELSE(AC_LANG_SOURCE([[
#			if HAVE_WINDOWS_H
#				include <windows.h>
#			endif
#			if HAVE_WINCRYPT_H
#				include <wincrypt.h>
#			endif

			int main()
			{
				HCRYPTPROV csp;
				int ret;

				if(!CryptAcquireContext(&csp, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET | CRYPT_SILENT))
					exit(1);
				if(!CryptReleaseContext(csp, 0))
					exit(2);
				exit(0);
			}
		]]), [dm_cv_lib_advapi32=yes], [dm_cv_lib_advapi32=no])
		LIBS="$dm_tmp_libs"
		AC_LANG_POP(C)
	])
	AS_IF([test x"$dm_cv_lib_advapi32" = x"yes"],[
		m4_if([$1], [], [
			AC_DEFINE(HAVE_LIBADVAPI32, 1, [define to 1 if your system has advapi32.dll])
			LIBS="$LIBS -ladvapi32"
		],[$1])
	],[
		m4_if([$2], [], [:], [$2])
	])
])dnl
