AC_DEFUN(RH_CHECK_FILE_LIST,
[
AC_MSG_CHECKING([for $4])
AC_CACHE_VAL(rh_cv_path_$1,
[case "[$]$1" in
  /*)
    rh_cv_path_$1="[$]$1" # Let the user override the test with a path.
  ;;
  ?:/*)
    rh_cv_path_$1="[$]$1" # Let the user override the test with a dos path.
  ;;
esac
if test -z $rh_cv_path_$1; then
  if test "$cross_compiling" = yes; then
    AC_MSG_ERROR(Cannot check for file existence when cross compiling)
  else
    for dir in $3; do
      if test -r $dir/$2; then
        eval "rh_cv_path_$1=$dir"
        break;
       else
        eval "rh_cv_path_$1=no"
      fi
    done    
  fi
fi
])dnl
$1="$rh_cv_path_$1"
AC_MSG_RESULT([$]$1)
if test "[$]$1" = no; then
  if test "x$5" = x; then
    AC_MSG_ERROR([Could not find $4.
    Please give me a hint by setting the variable $1])
  else
    AC_MSG_WARN([Could not find $4.
    $5])
  fi
fi
AC_SUBST($1)dnl
])
