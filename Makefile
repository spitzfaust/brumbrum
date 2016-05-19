# Makefile for OBES
# Thomas Rauhofer and Tobias Watzek
#                  .88888888:.
#                 88888888.88888.
#               .8888888888888888.
#               888888888888888888
#               88' _`88'_  `88888
#               88 88 88 88  88888
#               88_88_::_88_:88888
#               88:::,::,:::::8888
#               88`:::::::::'`8888
#              .88  `::::'    8:88.
#             8888            `8:888.
#           .8888'             `888888.
#          .8888:..  .::.  ...:'8888888:.
#         .8888.'     :'     `'::`88:88888
#        .8888        '         `.888:8888.
#       888:8         .           888:88888
#     .888:88        .:           888:88888:
#     8888888.       ::           88:888888
#     `.::.888.      ::          .88888888
#    .::::::.888.    ::         :::`8888'.:.
#   ::::::::::.888   '         .::::::::::::
#   ::::::::::::.8    '      .:8::::::::::::.
#  .::::::::::::::.        .:888:::::::::::::
#  :::::::::::::::88:.__..:88888:::::::::::'
#   `'.:::::::::::88888888888.88:::::::::'
# miK     `':::_:' -- '' -'-' `':_::::'`

CC=gcc
CFLAGS=-g -Wall -std=gnu11


all: testsignal myfifo mypipe mypopen vehicleclient gridserver

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

vehicleclient: vehicleclient.o
	${CC} ${CFLAGS} vehicleclient.o -o vehicleclient



gridserver: gridserver.o
	${CC} ${CFLAGS} gridserver.o -o gridserver

.PHONY: clean
clean:
	rm -f *.o myfifo mypipe mypopen vehicleclient gridserver testsignal

