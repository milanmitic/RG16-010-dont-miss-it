PROGRAM = DontMissIt
CC      = gcc
CFLAGS  = -g -ansi -Wall -std=c99 -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -L/usr/X11R6/lib -L/usr/pkg/lib
LDLIBS  = -lglut -lGLU -lGL -lm

$(PROGRAM): main.o draw.o
	$(CC) $(LDFLAGS) -o $(PROGRAM) $^ $(LDLIBS)

main.o: main.c draw.h
	$(CC) $(LDFLAGS) -c $< $(LDLIBS)

draw.o: draw.c draw.h
	$(CC) $(LDFLAGS) -c $< $(LDLIBS)

.PHONY: clean

clean:
	-rm *.o $(PROGRAM)

