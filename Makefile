PROGRAM = DontMissIt
CC      = gcc
CFLAGS  = -g -ansi -Wall -std=c99 -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -L/usr/X11R6/lib -L/usr/pkg/lib
LDLIBS  = -lglut -lGLU -lGL -lm

$(PROGRAM): main.o
	$(CC) $(LDFLAGS) -o $(PROGRAM) main.o $(LDLIBS)

.PHONY: clean

clean:
	-rm *.o $(PROGRAM)
