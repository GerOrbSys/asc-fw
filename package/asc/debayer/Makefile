CFLAGS = -Wall -O3

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o debayer main.c

forpc: $(OBJECTS)
	$(CC) $(CFLAGS) -o debayer main.c -lpng

clean:
	rm -f *.o debayer
