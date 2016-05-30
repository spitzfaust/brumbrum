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

all: testsignal myfifo mypipe mypopen vehicleclient gridserver griddisplay

testsignal: testsignal.c
	${CC} ${CFLAGS} testsignal.c -o testsignal

myfifo: myfifo.c
	${CC} ${CFLAGS} myfifo.c -o myfifo

mypipe: mypipe.c
	${CC} ${CFLAGS}  mypipe.c -o mypipe

mypopen: mypopen.c
	${CC} ${CFLAGS} mypopen.c -o mypopen

vehicleclient: vehicleclient.c
	${CC} ${CFLAGS} vehicleclient.c -o vehicleclient

gridserver: gridserver.c
	${CC} ${CFLAGS} gridserver.c -o gridserver

griddisplay: griddisplay.c
	${CC} ${CFLAGS} griddisplay.c -o griddisplay

.PHONY: clean
clean:
	rm -f *.o myfifo mypipe mypopen vehicleclient gridserver griddisplay testsignal
