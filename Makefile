.POSIX:

.SUFFIXES: .cat .msg

default: all

CFLAGS=-g -Wall -Wextra -Wpedantic -D_XOPEN_SOURCE=700

UTILITY=sh
SOURCES=alias.c bg.c builtins.c cd.c command.c fc.c fg.c getopts.c \
	hash.c jobs.c kill.c newgrp.c pwd.c read.c set.c main.c type.c \
	ulimit.c umask.c wait.c \
	dot.c eval.c exec.c exit.c export.c readonly.c shift.c times.c trap.c \
	unset.c \
	interactive.c parse.c init.c getopt.c \
	shed.c shed_commands.c shed_edit.c shed_insert.c shed_io.c shed_move.c shed_non_vi.c
HEADERS=sh.h shed.h
OBJECTS=alias.o bg.o builtins.o cd.o command.o fc.o fg.o getopts.o \
	hash.o jobs.o kill.o newgrp.o pwd.o read.o set.o main.o type.o \
	ulimit.o umask.o wait.o \
	dot.o eval.o exec.o exit.o export.o readonly.o shift.o times.o trap.o \
	unset.o \
	interactive.o parse.o init.o getopt.o \
	shed.o shed_commands.o shed_edit.o shed_insert.o shed_io.o shed_move.o shed_non_vi.o \
	sh.tab.o sh.yy.o
BUILTINS=alias bg cd command fc fg getopts jobs newgrp read \
	ulimit umask unalias wait

all: $(UTILITY) $(L10N) $(BUILTINS)

sh: $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(CFLAGS) $(OBJECTS)

$(BUILTINS): $(UTILITY)
	#ln -s $(UTILITY) $@
	for i in $(BUILTINS); do ln -s sh $$i; done

sh.yy.c: sh.l sh.tab.h
	$(LEX) -t sh.l > $@

sh.tab.h sh.tab.c: sh.y
	$(YACC) -b sh -tvd sh.y

clean:
	rm -f *.o $(L10N) $(UTILITY) $(BUILTINS) sh.tab.[ch] sh.yy.c
