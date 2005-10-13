# DM_YES_SUBST_1([VAR-TO-CEHCK], [VAR-TO-SUBST])
AC_DEFUN([DM_YES_SUBST_1],
[
	AS_IF([test x"$1" = x"yes"],
	[
		AC_SUBST([$2], [1])
	],[
		AC_SUBST([$2], [0])
	])
])
