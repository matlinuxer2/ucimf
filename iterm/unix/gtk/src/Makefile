TARGET = gtkiterm
OBJS = gtkiterm.o
CFLAGS = -g -Wall -I../../../lib/include
LD_FLAGS = -L../../../lib/src/.libs

RM = rm
all: $(TARGET)

$(OBJS): gtkiterm.c
	$(CC) $(CFLAGS) -c -o $@  -I../lib `pkg-config --cflags gtk+-2.0`  $<
$(TARGET): $(OBJS) ../lib/libgtkiterm.a
	gcc $(LD_FLAGS) $(CFLAGS) -o gtkiterm gtkiterm.c -I../lib `pkg-config --cflags --libs gtk+-2.0` ../lib/libgtkiterm.a -literm

../lib/libgtkiterm.a: ../lib/VTScreenView.c  ../lib/gtkiterm.c ../lib/VTScreenView.h  ../lib/gtkiterm.h
	$(MAKE) -C ../lib

clean:
	$(RM) -rf $(OBJS)
	$(RM) -rf $(TARGET)
	$(RM) -rf *~
