bin_PROGRAMS = tsh
tsh_SOURCES = main.c

check_SCRIPTS = greptest.sh
TESTS = $(check_SCRIPTS)

greptest.sh:
	@echo './tsh --version | grep "tsh version 1.1.0"' > greptest.sh
	@chmod +x greptest.sh

CLEANFILES = greptest.sh
