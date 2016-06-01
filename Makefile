# Makefile for OBES IPC Exercise
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
CFLAGS= -O3 -g -Wall -std=gnu11

all: vehicleclient gridserver griddisplay

vehicleclient: vehicleclient.o
	${CC} ${CFLAGS} vehicleclient.c -o vehicleclient

vehicleclient.o: vehicleclient.c
	${CC} ${CFLAGS} -c vehicleclient.c -o vehicleclient.o

gridserver: gridserver.o
	${CC} ${CFLAGS} gridserver.o -o gridserver

gridserver.o: gridserver.c
	${CC} ${CFLAGS} -c gridserver.c -o gridserver.o

griddisplay: griddisplay.o
	${CC} ${CFLAGS} griddisplay.o -o griddisplay

griddisplay.o: griddisplay.c
	${CC} ${CFLAGS} -c griddisplay.c -o griddisplay.o

.PHONY: clean
clean:
	rm -f *.o vehicleclient gridserver griddisplay
