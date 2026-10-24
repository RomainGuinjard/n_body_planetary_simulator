CC=gcc
CFLAGS=-Wall -Werror -I/usr/local/include
LDFLAGS=-lraylib -lm -ldl -lpthread -lGL -lX11
INCLUDES=-I..

SRC=main.c physics.c data.c scale.c rendering.c print_body.c tinyfiledialogs.c 
OBJ=$(SRC:.c=.o)

all: main.x

main.x: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) -lm

tinyfiledialogs.o: ../tinyfiledialogs.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ../tinyfiledialogs.c -o tinyfiledialogs.o

%.o: %.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -f *~ *.o

realclean: clean
	rm -f *.x