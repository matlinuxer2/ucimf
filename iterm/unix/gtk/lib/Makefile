LIBS = libgtkiterm.a
OBJS = gtkiterm.o VTScreenView.o
CFLAGS =-g -Wall -I../../../lib/include
CC = gcc

all: $(OBJS)
	ar ru $(LIBS) $(OBJS)

gtkiterm.o: gtkiterm.c gtkiterm.h
	$(CC) $(CFLAGS) -c -o $@ `pkg-config --cflags gtk+-2.0` gtkiterm.c

VTScreenView.o: VTScreenView.c VTScreenView.h
	$(CC) $(CFLAGS) -c -o $@ -g `pkg-config --cflags gtk+-2.0`  VTScreenView.c

clean:
	rm -rf $(OBJS)
	rm -rf $(LIBS)
	rm -rf *~
