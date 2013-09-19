bin_PROGRAMS = tsh
tsh_SOURCES = main.c
tsh_LDADD = -lreadline

check_SCRIPTS = greptest.sh
TESTS = $(check_SCRIPTS)

greptest.sh:
	@echo './tsh --version | grep "tsh version 1.2.0"' > greptest.sh
	@chmod +x greptest.sh

CLEANFILES = greptest.sh
