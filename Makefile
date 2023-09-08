CC = gcc
CFLAGS = -Wall -std=c99 -g

kiosk: kiosk.o menu.o input.o

kiosk.o: kiosk.c menu.o input.o
menu.o: menu.c menu.h input.o
input.o: input.c input.h

clean:
	rm -f *.o
	rm -f kiosk
	rm -f output*.txt
	rm -f stderr.txt
	rm -f stdout.txt
