.POSIX:

.SUFFIXES: .cat .msg

default: all

CFLAGS=-g -Wall -Wextra -Wpedantic -Werror

UTILITY=sh
SOURCES=alias.c bg.c builtins.c cd.c command.c false.c fc.c fg.c getopts.c \
	hash.c jobs.c kill.c newgrp.c pwd.c read.c set.c main.c true.c type.c \
	ulimit.c umask.c wait.c \
	dot.c eval.c exec.c exit.c export.c readonly.c shift.c times.c trap.c \
	unset.c \
	interactive.c parse.c init.c
HEADERS=sh.h
OBJECTS=alias.o bg.o builtins.o cd.o command.o false.o fc.o fg.o getopts.o \
	hash.o jobs.o kill.o newgrp.o pwd.o read.o set.o main.o true.o type.o \
	ulimit.o umask.o wait.o \
	dot.o eval.o exec.o exit.o export.o readonly.o shift.o times.o trap.o \
	unset.o \
	interactive.o parse.o init.o \
	sh.tab.o sh.yy.o
BUILTINS=alias bg cd command false fc fg getopts jobs kill newgrp pwd read \
	true ulimit umask unalias wait

all: $(UTILITY) $(L10N) $(BUILTINS)

sh: $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(CFLAGS) $(OBJECTS) -ly -ll

$(BUILTINS): $(UTILITY)
	#ln -s $(UTILITY) $@
	for i in $(BUILTINS); do ln -s sh $$i; done

sh.yy.c: sh.l sh.tab.h
	$(LEX) -t sh.l > $@

sh.tab.h sh.tab.c: sh.y
	$(YACC) -b sh -tvd sh.y

clean:
	rm -f *.o $(L10N) $(UTILITY) $(BUILTINS) sh.tab.[ch] sh.yy.c
