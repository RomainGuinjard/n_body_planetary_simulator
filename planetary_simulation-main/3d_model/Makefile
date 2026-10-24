CC=gcc
CFLAGS=-Wall -Werror -I/usr/local/include
LDFLAGS=-lraylib -lm -ldl -lpthread -lGL -lX11
INCLUDES=-I..

SRC=main3d.c physics3d.c data3d.c rendering3d.c scale3d.c print_body3d.c camera_utils3d.c tinyfiledialogs.c
OBJ=$(SRC:.c=.o)

all: main3d.x

main3d.x: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) -lm

tinyfiledialogs.o: ../tinyfiledialogs.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ../tinyfiledialogs.c -o tinyfiledialogs.o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -f *~ *.o

realclean: clean
	rm -f *.x
