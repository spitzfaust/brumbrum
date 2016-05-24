# Makefile for lab
# Thomas Rauhofer and Tobias Watzek
# d[ o_0 ]b

CC=gcc
CFLAGS=-g -Wall

all: testsignal myfifo mypipe mypopen  gridserver myqueue_client

testsignal: testsignal.o
	${CC} ${CFLAGS} testsignal.o -o testsignal

myfifo: myfifo.o
	${CC} ${CFLAGS} myfifo.o -o myfifo

myfifo.o: myfifo.c
	${CC} ${CFLAGS} -c myfifo.c -o myfifo.o

mypipe: mypipe.o
	${CC} ${CFLAGS} mypipe.o -o mypipe

mypipe.o: mypipe.c
	${CC} ${CFLAGS} -c mypipe.c -o mypipe.o

mypopen: mypopen.o
	${CC} ${CFLAGS} mypopen.o -o mypopen

mypopen.o: mypopen.c
	${CC} ${CFLAGS} -c mypopen.c -o mypopen.o

gridserver: gridserver.o
	${CC} ${CFLAGS} gridserver.o -o gridserver

myqueue_client: myqueue_client.o
	${CC} ${CFLAGS} myqueue_client.o -o myqueue_client
.PHONY: clean
clean:
	rm -f *.o myfifo mypipe mypopen gridserver myqueue_client
