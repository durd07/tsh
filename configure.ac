#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([tsh], [1.2], [Tyrion-Lannister-274AL@gmail.com])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
have_readline = no
AC_SEARCH_LIBS([readline add_history], [readline], [have_readline=yes])

if test "x${have_readline} = xyes"; then
   AC_CHECK_HEADERS([readline/readline.h readline/history.h], [], [have_readline=no])
fi

if test "x${have_readline}" = xno; then
   AC_MSG_ERROR([
   ----------------------------------------
   The readline library and header file
   required to build tsh. Stopping...
   Check 'config.log' for more information.
   ----------------------------------------])
fi

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h string.h \
                  unistd.h getopt.h errno.h \
                  sys/types.h sys/wait.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([getcwd getlogin_r fgets \
                strtok execvp])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

echo \
"-------------------------------------------------------
${PACKAGE_NAME} Version ${PACKAGE_VERSION}
 
Prefix: '${prefix}'.
Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'
Libraries: ${LIBS}

Now type 'make @<:@<target>@:>@'
    where the optional <target> is:
        all             - build all binaries
        install         - install everything
-------------------------------------------------------"
