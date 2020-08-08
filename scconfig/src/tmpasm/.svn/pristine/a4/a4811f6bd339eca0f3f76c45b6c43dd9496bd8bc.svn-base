CFLAGS = -Wall -g \
 -I../default -DTMPASM_TESTER \

tester: tester.o tmpasm.o debug.o tmpasm_scconfig.o openfiles.o \
 ../default/db.o ../default/ht.o ../default/str.o ../default/log.o \
 ../default/regex.o ../default/lib_uniqinc.o

tmpasm.o: tmpasm.c tmpasm.h

test: regression/Makefile
	cd regression && make

regression/Makefile: regression/Makefile.in tester
	./tester -e < regression/Makefile.in > regression/Makefile

debug.o: debug.c debug.h tmpasm.h
